#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanDevice.h"


struct SwapchainImage {
	VkImage image;
	VkImageView imageView;
};


class VulkanSwapchain
{
public:
	VulkanSwapchain(
		VulkanInstance& instance,
		VulkanDevice& device
	);
	void createSwapChain( GLFWwindow* window);
	VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
	VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR> presentationModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, GLFWwindow* window);

	void cleanup();

	VkSwapchainKHR swapchain;
	std::vector<SwapchainImage> swapChainImages;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

private:
	VulkanInstance& vulkanInstance;
	VulkanDevice& vulkanDevice;

};