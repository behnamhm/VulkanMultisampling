#pragma once


#include "stb_image.h"
#include <string>
#include <vector>


#include "vulkanAPI/VulkanBuffer.h"
#include "vulkanAPI/VulkanCommand.h"
#include "vulkanAPI/VulkanDevice.h"
#include "vulkanAPI/VulkanDescriptors.h"


class Texture
{
public:

	int createTextureImage(std::string fileName,VulkanBuffer& vulkanBuffer, 
							 VulkanCommand& vulkanCommand, VulkanDevice& vulkanDevice);
	int createTexture(std::string fileName,  VulkanBuffer& vulkanBuffer, 
						 VulkanCommand& vulkanCommand, VulkanDevice& vulkanDevice, VulkanDescriptors& vulkanDescriptors);
	int createTextureDescriptor(VkImageView textureImage,  
								VulkanDevice& vulkanDevice, VulkanDescriptors& vulkanDescriptors, VulkanBuffer& vulkanBuffer);

	// -- Loader Functions
	stbi_uc* loadTextureFile(std::string fileName, int* width, int* height, VkDeviceSize* imageSize);

	void createTextureSampler(VulkanDevice& vulkanDevice);

	void cleanup(VulkanDevice& vulkanDevice);

private:

	VkSampler textureSampler;
};