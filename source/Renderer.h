#pragma once


#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>
#include <array>

#include "components/Texture.h"
#include "components/MeshModel.h"
#include "VulkanAPI/VulkanValidation.h"
#include "VulkanAPI/VulkanSwapchain.h"
#include "VulkanAPI/VulkanPipeline.h"
#include "VulkanAPI/VulkanDescriptors.h"
#include "VulkanAPI/VulkanSync.h"
#include "VulkanAPI/VulkanCommand.h"
#include "VulkanAPI/VulkanRenderpass.h"
#include "VulkanAPI/VulkanInstance.h"
#include "VulkanAPI/VulkanBuffer.h"



class Renderer
{
public:
	Renderer();

	int init(GLFWwindow* newWindow);

	void draw();
	void update();

	void createFramebuffers();


	void recordCommands(uint32_t currentImage);

	~Renderer();

private:
	VulkanInstance vulkanInstance;
	VulkanDevice vulkanDevice;
	VulkanBuffer vulkanBuffer;
	VulkanSync vulkanSync;
	VulkanRenderpass vulkanRenderpass;
	VulkanDescriptors vulkanDescriptors;
	VulkanSwapchain vulkanSwapchain;
	VulkanPipeline vulkanPipeline;
	VulkanCommand vulkanCommand;


	UboViewProjection uboViewProjection;
	MeshModel model;
	GLFWwindow* window;
	Texture texture;

	int currentFrame = 0;
	int benchModel = 0;

	float angle = 0.0f;
	float deltaTime = 0.0f;
	float lastTime = 0.0f;

};

