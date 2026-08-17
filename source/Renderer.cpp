#include "Renderer.h"


Renderer::Renderer()
	: vulkanDevice(vulkanInstance),
	  vulkanSwapchain(vulkanInstance, vulkanDevice),
	  vulkanSync(vulkanDevice),
	  vulkanBuffer(vulkanDevice),
	  vulkanDescriptors(vulkanDevice),
	  vulkanRenderpass(vulkanDevice),
	  vulkanCommand(vulkanDevice),
	  vulkanPipeline(vulkanDevice,vulkanDescriptors, vulkanRenderpass)

{}

int Renderer::init(GLFWwindow* newWindow)
{
	window = newWindow;

	try {
		vulkanInstance.createInstance();
		vulkanInstance.createDebugCallback();
		vulkanInstance.createSurface( window);

		vulkanDevice.getPhysicalDevice();
		vulkanDevice.createLogicalDevice();

		vulkanSwapchain.createSwapChain(window);

		vulkanBuffer.createUniformBuffers(vulkanSwapchain.swapChainImages.size());

		vulkanRenderpass.createRenderPass(vulkanSwapchain.swapChainImageFormat);

		vulkanDescriptors.createDescriptorSetLayout();

		vulkanPipeline.createPushConstantRange(sizeof(model));

		vulkanPipeline.createGraphicsPipeline(vulkanSwapchain.swapChainExtent);

		vulkanBuffer.createColourBufferImage(vulkanSwapchain.swapChainImages.size(), vulkanSwapchain.swapChainExtent, vulkanSwapchain.swapChainImageFormat);
		vulkanBuffer.createDepthBufferImage(vulkanSwapchain.swapChainImages.size(), vulkanSwapchain.swapChainExtent);

		vulkanDescriptors.createDescriptorPool(vulkanSwapchain.swapChainImages.size(), 
											   vulkanBuffer.depthBufferImageView, 
												vulkanBuffer.colourBufferImageView);
		vulkanDescriptors.createDescriptorSets(vulkanSwapchain.swapChainImages.size());
		vulkanDescriptors.createInputDescriptorSets(vulkanSwapchain.swapChainImages.size(), 
													vulkanBuffer.depthBufferImageView,
													vulkanBuffer.colourBufferImageView);
		createFramebuffers();

		vulkanCommand.createCommandPool();
		vulkanCommand.createCommandBuffers(vulkanSwapchain.swapChainFramebuffers.size());

		vulkanSync.createSynchronisation(vulkanSwapchain.swapChainImages.size());
		
		texture.createTextureSampler(vulkanDevice);

		uboViewProjection.projection = glm::perspective(glm::radians(45.0f), (float)vulkanSwapchain.swapChainExtent.width / (float)vulkanSwapchain.swapChainExtent.height, 0.1f, 100.0f);
		uboViewProjection.view = glm::lookAt(glm::vec3(10.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, -17.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		uboViewProjection.projection[1][1] *= -1;


		benchModel = model.createMeshModel("assets/Models/Bench_HighRes.obj", vulkanBuffer, vulkanCommand, vulkanDevice);

		// Create our default "no texture" texture
		texture.createTexture("plain.png", vulkanBuffer, vulkanCommand, vulkanDevice, vulkanDescriptors);

	}
	catch (const std::runtime_error& e) {
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}

	return true;
}



void Renderer::draw()
{
	// -- GET NEXT IMAGE --
	// Wait for given fence to signal (open) from last draw before continuing
	vkWaitForFences(vulkanDevice.logicalDevice, 1, &vulkanSync.drawFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	// Manually reset (close) fences
	vkResetFences(vulkanDevice.logicalDevice, 1, &vulkanSync.drawFences[currentFrame]);

	// Get index of next image to be drawn to, and signal semaphore when ready to be drawn to
	uint32_t imageIndex;
	vkAcquireNextImageKHR(vulkanDevice.logicalDevice, vulkanSwapchain.swapchain, std::numeric_limits<uint64_t>::max(),
		vulkanSync.imageAvailable[currentFrame], VK_NULL_HANDLE, &imageIndex);

	recordCommands(imageIndex);
	vulkanBuffer.updateUniformBuffers(imageIndex,  uboViewProjection);

	// -- SUBMIT COMMAND BUFFER TO RENDER --
	// Queue submission information
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;										// Number of semaphores to wait on
	submitInfo.pWaitSemaphores = &vulkanSync.imageAvailable[currentFrame];				// List of semaphores to wait on
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};
	submitInfo.pWaitDstStageMask = waitStages;						// Stages to check semaphores at
	submitInfo.commandBufferCount = 1;								// Number of command buffers to submit
	submitInfo.pCommandBuffers = &vulkanCommand.commandBuffers[imageIndex];		// Command buffer to submit
	submitInfo.signalSemaphoreCount = 1;							// Number of semaphores to signal
	submitInfo.pSignalSemaphores = &vulkanSync.renderFinished[imageIndex];	// Semaphores to signal when command buffer finishes

	// Submit command buffer to queue
	VkResult result = vkQueueSubmit(vulkanDevice.graphicsQueue, 1, &submitInfo, vulkanSync.drawFences[currentFrame]);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit Command Buffer to Queue!");
	}


	// -- PRESENT RENDERED IMAGE TO SCREEN --
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;										// Number of semaphores to wait on
	presentInfo.pWaitSemaphores = &vulkanSync.renderFinished[imageIndex];			// Semaphores to wait on
	presentInfo.swapchainCount = 1;											// Number of swapchains to present to
	presentInfo.pSwapchains = &vulkanSwapchain.swapchain;									// Swapchains to present images to
	presentInfo.pImageIndices = &imageIndex;								// Index of images in swapchains to present

	// Present image
	result = vkQueuePresentKHR(vulkanDevice.presentationQueue, &presentInfo);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present Image!");
	}

	// Get next frame (use % swapChainImages.size() to keep value below swapChainImages.size())
	currentFrame = (currentFrame + 1) % MAX_FRAME_DRAWS;
}

void Renderer::update()
{

	float now = glfwGetTime();
	deltaTime = now - lastTime;
	lastTime = now;

	glm::mat4 testMat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	testMat = glm::rotate(testMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model.updateModel(benchModel, testMat);

}

void Renderer::recordCommands(uint32_t currentImage)
{
	// Information about how to begin each command buffer
	VkCommandBufferBeginInfo bufferBeginInfo = {};
	bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	// Information about how to begin a render pass (only needed for graphical applications)
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = vulkanRenderpass.renderPass;							// Render Pass to begin
	renderPassBeginInfo.renderArea.offset = { 0, 0 };						// Start point of render pass in pixels
	renderPassBeginInfo.renderArea.extent = vulkanSwapchain.swapChainExtent;				// Size of region to run render pass on (starting at offset)

	std::array<VkClearValue, 3> clearValues = {};
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].color = { 0.6f, 0.65f, 0.4f, 1.0f };
	clearValues[2].depthStencil.depth = 1.0f;

	renderPassBeginInfo.pClearValues = clearValues.data();					// List of clear values
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());

	renderPassBeginInfo.framebuffer = vulkanSwapchain.swapChainFramebuffers[currentImage];

	// Start recording commands to command buffer!
	VkResult result = vkBeginCommandBuffer(vulkanCommand.commandBuffers[currentImage], &bufferBeginInfo);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to start recording a Command Buffer!");
	}

	// Begin Render Pass
	vkCmdBeginRenderPass(vulkanCommand.commandBuffers[currentImage], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Bind Pipeline to be used in render pass
	vkCmdBindPipeline(vulkanCommand.commandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline.graphicsPipeline);

	for (size_t j = 0; j < model.modelList.size(); j++)
	{
		MeshModel& thisModel = model.modelList[j];

		vkCmdPushConstants(
			vulkanCommand.commandBuffers[currentImage],
			vulkanPipeline.pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT,		// Stage to push constants to
			0,								// Offset of push constants to update
			sizeof(Model),					// Size of data being pushed
			&thisModel.getModel());			// Actual data being pushed (can be array)

		for (size_t k = 0; k < thisModel.getMeshCount(); k++)
		{

			VkBuffer vertexBuffers[] = { thisModel.getMesh(k)->getVertexBuffer() };					// Buffers to bind
			VkDeviceSize offsets[] = { 0 };												// Offsets into buffers being bound
			vkCmdBindVertexBuffers(vulkanCommand.commandBuffers[currentImage], 0, 1, vertexBuffers, offsets);	// Command to bind vertex buffer before drawing with them

			// Bind mesh index buffer, with 0 offset and using the uint32 type
			vkCmdBindIndexBuffer(vulkanCommand.commandBuffers[currentImage], thisModel.getMesh(k)->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			// Dynamic Offset Amount
			// uint32_t dynamicOffset = static_cast<uint32_t>(modelUniformAlignment) * j;

			// "Push" constants to given shader stage directly (no buffer)


			std::array<VkDescriptorSet, 2> descriptorSetGroup = { vulkanDescriptors.descriptorSets[currentImage],
				vulkanDescriptors.samplerDescriptorSets[thisModel.getMesh(k)->getTexId()] };

			// Bind Descriptor Sets
			vkCmdBindDescriptorSets(vulkanCommand.commandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline.pipelineLayout,
				0, static_cast<uint32_t>(descriptorSetGroup.size()), descriptorSetGroup.data(), 0, nullptr);

			// Execute pipeline
			vkCmdDrawIndexed(vulkanCommand.commandBuffers[currentImage], thisModel.getMesh(k)->getIndexCount(), 1, 0, 0, 0);
		}
	}

	
	// End Render Pass
	vkCmdEndRenderPass(vulkanCommand.commandBuffers[currentImage]);

	// Stop recording to command buffer
	result = vkEndCommandBuffer(vulkanCommand.commandBuffers[currentImage]);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to stop recording a Command Buffer!");
	}

}


void Renderer::createFramebuffers()
{
	// Resize framebuffer count to equal swap chain image count
	vulkanSwapchain.swapChainFramebuffers.resize(vulkanSwapchain.swapChainImages.size());

	// Create a framebuffer for each swap chain image
	for (size_t i = 0; i < vulkanSwapchain.swapChainFramebuffers.size(); i++)
	{
		std::array<VkImageView, 3> attachments = {
			vulkanSwapchain.swapChainImages[i].imageView,
			vulkanBuffer.colourBufferImageView[i],
			vulkanBuffer.depthBufferImageView[i]
		};

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = vulkanRenderpass.renderPass;										// Render Pass layout the Framebuffer will be used with
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferCreateInfo.pAttachments = attachments.data();							// List of attachments (1:1 with Render Pass)
		framebufferCreateInfo.width = vulkanSwapchain.swapChainExtent.width;								// Framebuffer width
		framebufferCreateInfo.height = vulkanSwapchain.swapChainExtent.height;								// Framebuffer height
		framebufferCreateInfo.layers = 1;													// Framebuffer layers

		VkResult result = vkCreateFramebuffer(vulkanDevice.logicalDevice, &framebufferCreateInfo, nullptr, &vulkanSwapchain.swapChainFramebuffers[i]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create a Framebuffer!");
		}
	}
}



Renderer::~Renderer()
{
	vkDeviceWaitIdle(vulkanDevice.logicalDevice);

	vulkanDescriptors.cleanup();

	vulkanBuffer.cleanup();

	vulkanSync.cleanup();

	vulkanPipeline.cleanup();

	vulkanCommand.cleanup();

	vulkanSwapchain.cleanup();

	model.cleanup(vulkanDevice);
	texture.cleanup(vulkanDevice);
	vulkanRenderpass.cleanup();
	vulkanDevice.cleanup();

	vulkanInstance.cleanup();
}

