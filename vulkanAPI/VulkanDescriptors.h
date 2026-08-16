#pragma once


#include "VulkanDevice.h"


class VulkanDescriptors
{
public:
	VulkanDescriptors(
		VulkanDevice& vulkanDevice

	);

	void createDescriptorSetLayout();
	void createDescriptorPool(uint32_t swapChainSize, std::vector<VkImageView> &depthBufferImageView,
								std::vector<VkImageView>& colourBufferImageView);
	void createDescriptorSets(uint32_t swapChainSize);
	void createInputDescriptorSets(uint32_t swapChainSize, std::vector<VkImageView>& depthBufferImageView,
									std::vector<VkImageView>& colourBufferImageView);
	void cleanup();

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayout samplerSetLayout;
	VkDescriptorSetLayout inputSetLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorPool samplerDescriptorPool;
	VkDescriptorPool inputDescriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorSet> samplerDescriptorSets;
	std::vector<VkDescriptorSet> inputDescriptorSets;


private:
	VulkanDevice& vulkanDevice;
};