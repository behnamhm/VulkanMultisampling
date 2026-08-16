#pragma once


#include "VulkanDevice.h"

#include <array>

class VulkanCommand
{
public:
	VulkanCommand(
		VulkanDevice& vulkanDevice
	);

	void createCommandPool();
	void createCommandBuffers(uint32_t swapChainFrameBufferSize);
	static void copyBuffer(VkDevice device, VkQueue transferQueue, VkCommandPool transferCommandPool,
		VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize);
	static VkCommandBuffer beginCommandBuffer(VkDevice device, VkCommandPool commandPool);
	static void endAndSubmitCommandBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkCommandBuffer commandBuffer);
	static void copyImageBuffer(VkDevice device, VkQueue transferQueue, VkCommandPool transferCommandPool,
		VkBuffer srcBuffer, VkImage image, uint32_t width, uint32_t height);
	static void transitionImageLayout(VkDevice device, VkQueue queue, VkCommandPool commandPool, 
													 VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

	void cleanup();
	std::vector<VkCommandBuffer> commandBuffers;
	VkCommandPool graphicsCommandPool;

private:
	VulkanDevice& vulkanDevice;

};