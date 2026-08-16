#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"


#include "vulkanAPI/VulkanDevice.h"



class MeshModel
{
public:
	MeshModel();
	MeshModel(std::vector<Mesh> newMeshList);

	void updateModel(int modelId, glm::mat4 newModel);
	int createMeshModel(std::string modelFile,  VulkanBuffer& vulkanBuffer, VulkanCommand& vulkanCommand, VulkanDevice& vulkanDevice);
	size_t getMeshCount();
	Mesh * getMesh(size_t index);

	glm::mat4 getModel();
	void setModel(glm::mat4 newModel);

	void destroyMeshModel();

	std::vector<std::string> LoadMaterials(const aiScene * scene);
	std::vector<Mesh> LoadNode(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue, VkCommandPool transferCommandPool,
								aiNode * node, const aiScene * scene, std::vector<int> matToTex, VulkanBuffer& vulkanBuffer, VulkanCommand& vulkanCommand);
	Mesh LoadMesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue, VkCommandPool transferCommandPool,
					aiMesh * mesh, const aiScene * scene, std::vector<int> matToTex, VulkanBuffer& vulkanBuffer, VulkanCommand& vulkanCommand);
	void cleanup(VulkanDevice& vulkanDevice);
	std::vector<MeshModel> modelList;
	~MeshModel();

private:
	std::vector<Mesh> meshList;
	glm::mat4 model;

};

