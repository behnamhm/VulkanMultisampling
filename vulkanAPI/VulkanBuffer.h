#pragma once


#include "VulkanDevice.h"

#include <array>



class VulkanBuffer
{
public:
	VulkanBuffer(
		VulkanDevice& device

	);

	void createUniformBuffers(uint32_t swapChainSize);
	void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage,
						    VkMemoryPropertyFlags bufferProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

	void updateUniformBuffers(uint32_t imageIndex,  UboViewProjection& uboViewProjection);

	void createColourBufferImage(uint32_t swapChainSize, VkExtent2D& swapChainExtent);
	void createDepthBufferImage(uint32_t swapChainSize, VkExtent2D& swapChainExtent);


	void cleanup();

	std::vector<VkImageView> textureImageViews;
	std::vector<VkImage> textureImages;
	std::vector<VkDeviceMemory> textureImageMemory;
	std::vector<VkImageView> depthBufferImageView;
	std::vector<VkImageView> colourBufferImageView;
private:
	VulkanDevice& vulkanDevice;

	std::vector<VkDeviceMemory> depthBufferImageMemory;
	std::vector<VkImage> depthBufferImage;
	std::vector<VkImage> colourBufferImage;
	std::vector<VkDeviceMemory> colourBufferImageMemory;
};