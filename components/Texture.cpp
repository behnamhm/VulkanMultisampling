#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



int Texture::createTexture(std::string fileName,  VulkanBuffer& vulkanBuffer, 
							 VulkanCommand& vulkanCommand, VulkanDevice& vulkanDevice, VulkanDescriptors& vulkanDescriptors)
{
	// Create Texture Image and get its location in array
	int textureImageLoc = createTextureImage(fileName, vulkanBuffer,  vulkanCommand, vulkanDevice);

	// Create Image View and add to list
	VkImageView imageView = vulkanDevice.createImageView(vulkanBuffer.textureImages[textureImageLoc],
															VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	vulkanBuffer.textureImageViews.push_back(imageView);

	// Create Texture Descriptor
	int descriptorLoc = createTextureDescriptor(imageView,  vulkanDevice, vulkanDescriptors, vulkanBuffer);

	// Return location of set with texture
	return descriptorLoc;
}

int Texture::createTextureDescriptor(VkImageView textureImage, VulkanDevice& vulkanDevice, 
									VulkanDescriptors& vulkanDescriptors, VulkanBuffer& vulkanBuffer)
{
	VkDescriptorSet descriptorSet;

	// Descriptor Set Allocation Info
	VkDescriptorSetAllocateInfo setAllocInfo = {};
	setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocInfo.descriptorPool = vulkanDescriptors.samplerDescriptorPool;
	setAllocInfo.descriptorSetCount = 1;
	setAllocInfo.pSetLayouts = &vulkanDescriptors.samplerSetLayout;

	// Allocate Descriptor Sets
	VkResult result = vkAllocateDescriptorSets(vulkanDevice.logicalDevice, &setAllocInfo, &descriptorSet);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Texture Descriptor Sets!");
	}

	// Texture Image Info
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;	// Image layout when in use
	imageInfo.imageView = textureImage;									// Image to bind to set
	imageInfo.sampler = textureSampler;									// Sampler to use for set

	// Descriptor Write Info
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;

	// Update new descriptor set
	vkUpdateDescriptorSets(vulkanDevice.logicalDevice, 1, &descriptorWrite, 0, nullptr);

	// Add descriptor set to list
	vulkanDescriptors.samplerDescriptorSets.push_back(descriptorSet);

	// Return descriptor set location
	return vulkanDescriptors.samplerDescriptorSets.size() - 1;
}

int Texture::createTextureImage(std::string fileName, VulkanBuffer &vulkanBuffer, 
								VulkanCommand& vulkanCommand, VulkanDevice& vulkanDevice)
{
	// Load image file
	int width, height;
	VkDeviceSize imageSize;
	stbi_uc* imageData = loadTextureFile(fileName, &width, &height, &imageSize);

	// Create staging buffer to hold loaded data, ready to copy to device
	VkBuffer imageStagingBuffer;
	VkDeviceMemory imageStagingBufferMemory;
	vulkanBuffer.createBuffer(vulkanDevice.physicalDevice, vulkanDevice.logicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&imageStagingBuffer, &imageStagingBufferMemory);

	// Copy image data to staging buffer
	void* data;
	vkMapMemory(vulkanDevice.logicalDevice, imageStagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, imageData, static_cast<size_t>(imageSize));
	vkUnmapMemory(vulkanDevice.logicalDevice, imageStagingBufferMemory);

	// Free original image data
	stbi_image_free(imageData);

	// Create image to hold final texture
	VkImage texImage;
	VkDeviceMemory texImageMemory;
	texImage = vulkanDevice.createImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texImageMemory);


	// COPY DATA TO IMAGE
	// Transition image to be DST for copy operation
	vulkanCommand.transitionImageLayout(vulkanDevice.logicalDevice, vulkanDevice.graphicsQueue, vulkanCommand.graphicsCommandPool,
		texImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// Copy image data
	vulkanCommand.copyImageBuffer(vulkanDevice.logicalDevice, vulkanDevice.graphicsQueue, vulkanCommand.graphicsCommandPool, imageStagingBuffer, texImage, width, height);

	// Transition image to be shader readable for shader usage
	vulkanCommand.transitionImageLayout(vulkanDevice.logicalDevice, vulkanDevice.graphicsQueue, vulkanCommand.graphicsCommandPool,
		texImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	// Add texture data to vector for reference
	vulkanBuffer.textureImages.push_back(texImage);
	vulkanBuffer.textureImageMemory.push_back(texImageMemory);

	// Destroy staging buffers
	vkDestroyBuffer(vulkanDevice.logicalDevice, imageStagingBuffer, nullptr);
	vkFreeMemory(vulkanDevice.logicalDevice, imageStagingBufferMemory, nullptr);

	// Return index of new texture image
	return  vulkanBuffer.textureImages.size() - 1;
}

stbi_uc* Texture::loadTextureFile(std::string fileName, int* width, int* height, VkDeviceSize* imageSize)
{
	// Number of channels image uses
	int channels;

	// Load pixel data for image
	std::string fileLoc = "assets/textures/" + fileName;
	stbi_uc* image = stbi_load(fileLoc.c_str(), width, height, &channels, STBI_rgb_alpha);

	if (!image)
	{
		throw std::runtime_error("Failed to load a Texture file! (" + fileName + ")");
	}

	// Calculate image size using given and known data
	*imageSize = *width * *height * 4;

	return image;
}



void Texture::createTextureSampler(VulkanDevice& vulkanDevice)
{
	// Sampler Creation Info
	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;						// How to render when image is magnified on screen
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;						// How to render when image is minified on screen
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;	// How to handle texture wrap in U (x) direction
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;	// How to handle texture wrap in V (y) direction
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;	// How to handle texture wrap in W (z) direction
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;	// Border beyond texture (only workds for border clamp)
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;				// Whether coords should be normalized (between 0 and 1)
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;		// Mipmap interpolation mode
	samplerCreateInfo.mipLodBias = 0.0f;								// Level of Details bias for mip level
	samplerCreateInfo.minLod = 0.0f;									// Minimum Level of Detail to pick mip level
	samplerCreateInfo.maxLod = 0.0f;									// Maximum Level of Detail to pick mip level
	samplerCreateInfo.anisotropyEnable = VK_TRUE;						// Enable Anisotropy
	samplerCreateInfo.maxAnisotropy = 16;								// Anisotropy sample level

	VkResult result = vkCreateSampler(vulkanDevice.logicalDevice, &samplerCreateInfo, nullptr, &textureSampler);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Filed to create a Texture Sampler!");
	}
}

void Texture::cleanup(VulkanDevice& vulkanDevice)
{
	vkDestroySampler(
		vulkanDevice.logicalDevice,
		textureSampler,
		nullptr
	);
}

