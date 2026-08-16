#include "VulkanBuffer.h"

VulkanBuffer::VulkanBuffer(
	VulkanDevice& device
)
	: vulkanDevice(device)

{

}


void VulkanBuffer::createUniformBuffers(uint32_t swapChainSize)
{
	// ViewProjection buffer size
	VkDeviceSize vpBufferSize = sizeof(UboViewProjection);

	// Model buffer size
	//VkDeviceSize modelBufferSize = modelUniformAlignment * MAX_OBJECTS;

	// One uniform buffer for each image (and by extension, command buffer)
	vulkanDevice.vpUniformBuffer.resize(swapChainSize);
	vulkanDevice.vpUniformBufferMemory.resize(swapChainSize);
	//modelDUniformBuffer.resize(swapChainImages.size());
	//modelDUniformBufferMemory.resize(swapChainImages.size());

	// Create Uniform buffers
	for (size_t i = 0; i < swapChainSize; i++)
	{
		createBuffer(vulkanDevice.physicalDevice, vulkanDevice.logicalDevice, vpBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vulkanDevice.vpUniformBuffer[i],
			&vulkanDevice.vpUniformBufferMemory[i]);

		/*createBuffer(mainDevice.physicalDevice, mainDevice.logicalDevice, modelBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &modelDUniformBuffer[i], &modelDUniformBufferMemory[i]);*/
	}
}



void VulkanBuffer::createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage,
								VkMemoryPropertyFlags bufferProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
	// CREATE VERTEX BUFFER
	// Information to create a buffer (doesn't include assigning memory)
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;								// Size of buffer (size of 1 vertex * number of vertices)
	bufferInfo.usage = bufferUsage;								// Multiple types of buffer possible
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;			// Similar to Swap Chain images, can share vertex buffers

	VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, buffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Vertex Buffer!");
	}

	// GET BUFFER MEMORY REQUIREMENTS
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

	// ALLOCATE MEMORY TO BUFFER
	VkMemoryAllocateInfo memoryAllocInfo = {};
	memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocInfo.allocationSize = memRequirements.size;
	memoryAllocInfo.memoryTypeIndex = vulkanDevice.findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits,		// Index of memory type on Physical Device that has required bit flags
		bufferProperties);																						// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT	: CPU can interact with memory
	// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	: Allows placement of data straight into buffer after mapping (otherwise would have to specify manually)
	// Allocate memory to VkDeviceMemory
	result = vkAllocateMemory(device, &memoryAllocInfo, nullptr, bufferMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Vertex Buffer Memory!");
	}

	// Allocate memory to given vertex buffer
	vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
}



void VulkanBuffer::updateUniformBuffers(uint32_t imageIndex,  UboViewProjection& uboViewProjection)
{
	// Copy VP data
	void* data;
	vkMapMemory(vulkanDevice.logicalDevice, vulkanDevice.vpUniformBufferMemory[imageIndex], 0, sizeof(UboViewProjection), 0, &data);
	memcpy(data, &uboViewProjection, sizeof(UboViewProjection));
	vkUnmapMemory(vulkanDevice.logicalDevice, vulkanDevice.vpUniformBufferMemory[imageIndex]);

	// Copy Model data
	/*for (size_t i = 0; i < meshList.size(); i++)
	{
		UboModel * thisModel = (UboModel *)((uint64_t)modelTransferSpace + (i * modelUniformAlignment));
		*thisModel = meshList[i].getModel();
	}

	// Map the list of model data
	vkMapMemory(mainDevice.logicalDevice, modelDUniformBufferMemory[imageIndex], 0, modelUniformAlignment * meshList.size(), 0, &data);
	memcpy(data, modelTransferSpace, modelUniformAlignment * meshList.size());
	vkUnmapMemory(mainDevice.logicalDevice, modelDUniformBufferMemory[imageIndex]);*/
}




void VulkanBuffer::createDepthBufferImage(uint32_t swapChainSize, VkExtent2D& swapChainExtent)
{
	depthBufferImage.resize(swapChainSize);
	depthBufferImageMemory.resize(swapChainSize);
	depthBufferImageView.resize(swapChainSize);

	// Get supported format for depth buffer
	VkFormat depthFormat = vulkanDevice.chooseSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

	for (size_t i = 0; i < swapChainSize; i++)
	{
		// Create Depth Buffer Image
		depthBufferImage[i] = vulkanDevice.createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&depthBufferImageMemory[i]);

		// Create Depth Buffer Image View
		depthBufferImageView[i] = vulkanDevice.createImageView(depthBufferImage[i], depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}
}

void VulkanBuffer::createColourBufferImage(uint32_t swapChainSize, VkExtent2D &swapChainExtent)
{
	// Resize supported format for colour attachment
	colourBufferImage.resize(swapChainSize);
	colourBufferImageMemory.resize(swapChainSize);
	colourBufferImageView.resize(swapChainSize);

	// Get supported format for colour attachment
	VkFormat colourFormat = vulkanDevice.chooseSupportedFormat(
		{ VK_FORMAT_R8G8B8A8_UNORM },
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);

	for (size_t i = 0; i < swapChainSize; i++)
	{
		// Create Colour Buffer Image
		colourBufferImage[i] = vulkanDevice.createImage(swapChainExtent.width, swapChainExtent.height, colourFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&colourBufferImageMemory[i]);

		// Create Colour Buffer Image View
		colourBufferImageView[i] = vulkanDevice.createImageView(colourBufferImage[i], colourFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}




void VulkanBuffer::cleanup()
{
	for (size_t i = 0; i < vulkanDevice.vpUniformBuffer.size(); i++)
	{
		vkDestroyBuffer(
			vulkanDevice.logicalDevice,
			vulkanDevice.vpUniformBuffer[i],
			nullptr
		);

		vkFreeMemory(
			vulkanDevice.logicalDevice,
			vulkanDevice.vpUniformBufferMemory[i],
			nullptr
		);
	}

	
	for (auto imageView : textureImageViews)
	{
		vkDestroyImageView(
			vulkanDevice.logicalDevice,
			imageView,
			nullptr
		);
	}

	for (auto image : textureImages)
	{
		vkDestroyImage(
			vulkanDevice.logicalDevice,
			image,
			nullptr
		);
	}

	for (auto memory : textureImageMemory)
	{
		vkFreeMemory(
			vulkanDevice.logicalDevice,
			memory,
			nullptr
		);
	}
	for (auto image : colourBufferImage)
		vkDestroyImage(vulkanDevice.logicalDevice, image, nullptr);

	for (auto memory : colourBufferImageMemory)
		vkFreeMemory(vulkanDevice.logicalDevice, memory, nullptr);

	for (auto image : depthBufferImage)
		vkDestroyImage(vulkanDevice.logicalDevice, image, nullptr);

	for (auto memory : depthBufferImageMemory)
		vkFreeMemory(vulkanDevice.logicalDevice, memory, nullptr);
	for (auto imageView : colourBufferImageView)
		vkDestroyImageView(vulkanDevice.logicalDevice, imageView, nullptr);
	for (auto imageView : depthBufferImageView)
		vkDestroyImageView(vulkanDevice.logicalDevice, imageView, nullptr);
}