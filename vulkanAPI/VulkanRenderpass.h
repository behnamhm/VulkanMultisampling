#pragma once

#include <array>


#include "VulkanDevice.h"


class VulkanRenderpass
{
public:
	VulkanRenderpass(
		VulkanDevice& vulkanDevice

	);
	void createRenderPass(VkFormat& swapChainImageFormat);
	void cleanup();

	VkRenderPass renderPass;

private:
	VulkanDevice& vulkanDevice;

};