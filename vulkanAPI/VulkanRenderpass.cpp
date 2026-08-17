#include "VulkanRenderpass.h"



VulkanRenderpass::VulkanRenderpass(
	VulkanDevice& device


)
	: vulkanDevice(device)

{}


void VulkanRenderpass::createRenderPass(VkFormat& swapChainImageFormat)
{
    std::array<VkSubpassDescription, 1> subpasses{};


    // ATTACHMENT 0: SWAPCHAIN (1x)
    VkAttachmentDescription swapchainColourAttachment{};
    swapchainColourAttachment.format = swapChainImageFormat;
    swapchainColourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    swapchainColourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    swapchainColourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    swapchainColourAttachment.stencilLoadOp =
        VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    swapchainColourAttachment.stencilStoreOp =
        VK_ATTACHMENT_STORE_OP_DONT_CARE;

    swapchainColourAttachment.initialLayout =
        VK_IMAGE_LAYOUT_UNDEFINED;

    swapchainColourAttachment.finalLayout =
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    // ATTACHMENT 1: MSAA COLOR (8x)

    VkAttachmentDescription colourAttachment{};

    colourAttachment.format =
        swapChainImageFormat;

    colourAttachment.samples = vulkanDevice.msaaSamples;
    colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colourAttachment.stencilLoadOp =
        VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colourAttachment.stencilStoreOp =
        VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colourAttachment.initialLayout =
        VK_IMAGE_LAYOUT_UNDEFINED;

    colourAttachment.finalLayout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // ATTACHMENT 2: MSAA DEPTH (8x)
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format =
        vulkanDevice.chooseSupportedFormat(
            { VK_FORMAT_D32_SFLOAT_S8_UINT,
              VK_FORMAT_D32_SFLOAT,
              VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    depthAttachment.samples = vulkanDevice.msaaSamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    depthAttachment.stencilLoadOp =
        VK_ATTACHMENT_LOAD_OP_DONT_CARE;

    depthAttachment.stencilStoreOp =
        VK_ATTACHMENT_STORE_OP_DONT_CARE;

    depthAttachment.initialLayout =
        VK_IMAGE_LAYOUT_UNDEFINED;

    depthAttachment.finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // ATTACHMENT REFERENCES
    VkAttachmentReference colourAttachmentReference{};
    colourAttachmentReference.attachment = 1;
    colourAttachmentReference.layout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    VkAttachmentReference depthAttachmentReference{};
    depthAttachmentReference.attachment = 2;
    depthAttachmentReference.layout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    // Resolve 8x color -> 1x swapchain
    VkAttachmentReference resolveAttachmentReference{};
    resolveAttachmentReference.attachment = 0;
    resolveAttachmentReference.layout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // SUBPASS
    subpasses[0].pipelineBindPoint =
        VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpasses[0].colorAttachmentCount = 1;

    subpasses[0].pColorAttachments =
        &colourAttachmentReference;

    subpasses[0].pDepthStencilAttachment =
        &depthAttachmentReference;

    subpasses[0].pResolveAttachments =
        &resolveAttachmentReference;

    // DEPENDENCIES

    std::array<VkSubpassDependency, 2> subpassDependencies{};

    // External -> subpass 0
    subpassDependencies[0].srcSubpass =
        VK_SUBPASS_EXTERNAL;

    subpassDependencies[0].srcStageMask =
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    subpassDependencies[0].srcAccessMask =
        VK_ACCESS_MEMORY_READ_BIT;

    subpassDependencies[0].dstSubpass = 0;

    subpassDependencies[0].dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    subpassDependencies[0].dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


    // Subpass 0 -> External
    subpassDependencies[1].srcSubpass = 0;

    subpassDependencies[1].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    subpassDependencies[1].srcAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    subpassDependencies[1].dstSubpass =
        VK_SUBPASS_EXTERNAL;

    subpassDependencies[1].dstStageMask =
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    subpassDependencies[1].dstAccessMask =
        VK_ACCESS_MEMORY_READ_BIT;


    // --------------------------------------------------
    // RENDER PASS
    // --------------------------------------------------

    std::array<VkAttachmentDescription, 3> renderPassAttachments =
    {
        swapchainColourAttachment,
        colourAttachment,
        depthAttachment
    };

    VkRenderPassCreateInfo renderPassCreateInfo{};

    renderPassCreateInfo.sType =
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    renderPassCreateInfo.attachmentCount =
        static_cast<uint32_t>(
            renderPassAttachments.size());

    renderPassCreateInfo.pAttachments =
        renderPassAttachments.data();

    renderPassCreateInfo.subpassCount = 1;

    renderPassCreateInfo.pSubpasses =
        subpasses.data();

    renderPassCreateInfo.dependencyCount =
        static_cast<uint32_t>(
            subpassDependencies.size());

    renderPassCreateInfo.pDependencies =
        subpassDependencies.data();

    VkResult result = vkCreateRenderPass(
        vulkanDevice.logicalDevice,
        &renderPassCreateInfo,
        nullptr,
        &renderPass);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create a Render Pass!");
    }
}


void VulkanRenderpass::cleanup()
{
	vkDestroyRenderPass(
		vulkanDevice.logicalDevice,
		renderPass,
		nullptr
	);

}