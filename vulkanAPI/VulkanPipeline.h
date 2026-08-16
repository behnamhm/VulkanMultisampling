#pragma once


#include "VulkanDevice.h"
#include "VulkanDescriptors.h"
#include "VulkanRenderpass.h"
#include "components/FileSystem.h"

#include <array>

class VulkanPipeline
{
public:
    VulkanPipeline(
        VulkanDevice& vulkanDevice,
        VulkanDescriptors& vulkanDescriptors,
        VulkanRenderpass& vulkanRenderpass
    );
    void createGraphicsPipeline(VkExtent2D& swapChainExtent);

    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createPushConstantRange(int size);
    void cleanup();
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;

    VkPipeline secondPipeline;
    VkPipelineLayout secondPipelineLayout;
    VkPushConstantRange pushConstantRange;

private:
    FileSystem fileSystem;
    VulkanDevice& vulkanDevice;
    VulkanDescriptors& vulkanDescriptors;
    VulkanRenderpass& vulkanRenderpass;
};