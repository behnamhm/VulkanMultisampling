#include "VulkanSync.h"



VulkanSync::VulkanSync(
	VulkanDevice& device

)
	: vulkanDevice(device)

{}


void VulkanSync::createSynchronisation(uint32_t swapChainSize)
{
	// Resources associated with frames in flight
	imageAvailable.resize(MAX_FRAME_DRAWS);
	drawFences.resize(MAX_FRAME_DRAWS);

	// Resources associated with swapchain images
	renderFinished.resize(swapChainSize);


	// Semaphore creation information
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;


	// Fence creation information
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;



	for (size_t i = 0; i < MAX_FRAME_DRAWS; i++)
	{
		if (vkCreateSemaphore(
			vulkanDevice.logicalDevice,
			&semaphoreCreateInfo,
			nullptr,
			&imageAvailable[i]) != VK_SUCCESS)
		{
			throw std::runtime_error(
				"Failed to create imageAvailable semaphore!"
			);
		}

		if (vkCreateFence(
			vulkanDevice.logicalDevice,
			&fenceCreateInfo,
			nullptr,
			&drawFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error(
				"Failed to create draw fence!"
			);
		}
	}



	for (size_t i = 0; i < swapChainSize; i++)
	{
		if (vkCreateSemaphore(
			vulkanDevice.logicalDevice,
			&semaphoreCreateInfo,
			nullptr,
			&renderFinished[i]) != VK_SUCCESS)
		{
			throw std::runtime_error(
				"Failed to create renderFinished semaphore!"
			);
		}
	}
}

void VulkanSync::cleanup()
{

	for (size_t i = 0; i < MAX_FRAME_DRAWS; i++)
	{
		vkDestroySemaphore(
			vulkanDevice.logicalDevice,
			imageAvailable[i],
			nullptr
		);

		vkDestroyFence(
			vulkanDevice.logicalDevice,
			drawFences[i],
			nullptr
		);
	}

	for (auto semaphore : renderFinished)
	{
		vkDestroySemaphore(
			vulkanDevice.logicalDevice,
			semaphore,
			nullptr
		);
	}

}