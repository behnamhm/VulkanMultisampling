#pragma once

#include "VulkanDevice.h"
#include <stdexcept>


class VulkanSync
{
public:
	VulkanSync(
		VulkanDevice& vulkanDevice
	);

	void createSynchronisation(uint32_t swapChainSize);
	void cleanup();

	std::vector<VkSemaphore> imageAvailable;
	std::vector<VkSemaphore> renderFinished;
	std::vector<VkFence> drawFences;


private:
	VulkanDevice& vulkanDevice;
};