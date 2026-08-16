#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include "VulkanValidation.h"


class VulkanInstance
{
public:
    void createInstance();
    void createDebugCallback();
    void createSurface( GLFWwindow* window);
    bool checkValidationLayerSupport();
    bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
    void cleanup();
    VkInstance instance;
    VkDebugReportCallbackEXT callback;

    VkSurfaceKHR surface;
};