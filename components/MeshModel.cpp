#include "MeshModel.h"



MeshModel::MeshModel()
{
}

MeshModel::MeshModel(std::vector<Mesh> newMeshList)
{
	meshList = newMeshList;
	model = glm::mat4(1.0f);
}

size_t MeshModel::getMeshCount()
{
	return meshList.size();
}

Mesh * MeshModel::getMesh(size_t index)
{
	if (index >= meshList.size())
	{
		throw std::runtime_error("Attempted to access invalid Mesh index!");
	}

	return &meshList[index];
}

glm::mat4 MeshModel::getModel()
{
	return model;
}

void MeshModel::setModel(glm::mat4 newModel)
{
	model = newModel;
}

void MeshModel::destroyMeshModel()
{
	for (auto &mesh : meshList)
	{
		mesh.destroyBuffers();
	}
}

std::vector<std::string> MeshModel::LoadMaterials(const aiScene * scene)
{
	// Create 1:1 sized list of textures
	std::vector<std::string> textureList(scene->mNumMaterials);

	// Go through each material and copy its texture file name (if it exists)
	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		// Get the material
		aiMaterial * material = scene->mMaterials[i];

		// Initialise the texture to empty string (will be replaced if texture exists)
		textureList[i] = "";

		// Check for a Diffuse Texture (standard detail texture)
		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			// Get the path of the texture file
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				// Cut off any directory information aleady present
				int idx = std::string(path.data).rfind("\\");
				std::string fileName = std::string(path.data).substr(idx + 1);

				textureList[i] = fileName;
			}
		}
	}

	return textureList;
}

std::vector<Mesh> MeshModel::LoadNode(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, 
							aiNode * node, const aiScene * scene, std::vector<int> matToTex, VulkanBuffer& vulkanBuffer, VulkanCommand& vulkanCommand)
{
	std::vector<Mesh> meshList;

	// Go through each mesh at this node and create it, then add it to our meshList
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		meshList.push_back(
			LoadMesh(newPhysicalDevice, newDevice, transferQueue, transferCommandPool, scene->mMeshes[node->mMeshes[i]], scene, matToTex, vulkanBuffer, vulkanCommand)
		);
	}

	// Go through each node attached to this node and load it, then append their meshes to this node's mesh list
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		std::vector<Mesh> newList = LoadNode(newPhysicalDevice, newDevice, transferQueue, transferCommandPool, node->mChildren[i], scene, matToTex, vulkanBuffer, vulkanCommand);
		meshList.insert(meshList.end(), newList.begin(), newList.end());
	}

	return meshList;
}

Mesh MeshModel::LoadMesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, 
						aiMesh * mesh, const aiScene * scene, std::vector<int> matToTex, VulkanBuffer& vulkanBuffer, VulkanCommand& vulkanCommand)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	// Resize vertex list to hold all vertices for mesh
	vertices.resize(mesh->mNumVertices);

	// Go through each vertex and copy it across to our vertices
	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		// Set position
		vertices[i].pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

		// Set tex coords (if they exist)
		if (mesh->mTextureCoords[0])
		{
			vertices[i].tex = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		}
		else
		{
			vertices[i].tex = { 0.0f, 0.0f };
		}

		// Set colour (just use white for now)
		vertices[i].col = { 1.0f, 1.0f, 1.0f };
	}

	// Iterate over indices through faces and copy across
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		// Get a face
		aiFace face = mesh->mFaces[i];

		// Go through face's indices and add to list
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Create new mesh with details and return it
	Mesh newMesh = Mesh(newPhysicalDevice, newDevice, transferQueue, transferCommandPool, &vertices, &indices, 
						matToTex[mesh->mMaterialIndex], vulkanBuffer, vulkanCommand);

	return newMesh;
}

void MeshModel::updateModel(int modelId, glm::mat4 newModel)
{
	if (modelId >= modelList.size()) return;

	modelList[modelId].setModel(newModel);
}

int MeshModel::createMeshModel(std::string modelFile,  VulkanBuffer& vulkanBuffer, VulkanCommand& vulkanCommand, VulkanDevice& vulkanDevice)
{
	// Import model "scene"
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelFile, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	if (!scene)
	{
		throw std::runtime_error("Failed to load model! (" + modelFile + ")");
	}

	// Get vector of all materials with 1:1 ID placement
	std::vector<std::string> textureNames = MeshModel::LoadMaterials(scene);

	// Conversion from the materials list IDs to our Descriptor Array IDs
	std::vector<int> matToTex(textureNames.size());

	// Loop over textureNames and create textures for them
	for (size_t i = 0; i < textureNames.size(); i++)
	{
		// If material had no texture, set '0' to indicate no texture, texture 0 will be reserved for a default texture
		if (textureNames[i].empty())
		{
			matToTex[i] = 0;
		}

	}

	// Load in all our meshes
	std::vector<Mesh> modelMeshes = MeshModel::LoadNode(vulkanDevice.physicalDevice, vulkanDevice.logicalDevice,
		vulkanDevice.graphicsQueue, vulkanCommand.graphicsCommandPool,
		scene->mRootNode, scene, matToTex, vulkanBuffer, vulkanCommand);

	// Create mesh model and add to list
	MeshModel meshModel = MeshModel(modelMeshes);
	modelList.push_back(meshModel);

	return modelList.size() - 1;
}


void MeshModel::cleanup(VulkanDevice& vulkanDevice)
{

	for (auto& model : modelList)
	{
		for (size_t i = 0; i < model.getMeshCount(); i++)
		{
			auto mesh = model.getMesh(i);

			vkDestroyBuffer(
				vulkanDevice.logicalDevice,
				mesh->getVertexBuffer(),
				nullptr
			);

			vkFreeMemory(
				vulkanDevice.logicalDevice,
				mesh->getVertexBufferMemory(),
				nullptr
			);

			vkDestroyBuffer(
				vulkanDevice.logicalDevice,
				mesh->getIndexBuffer(),
				nullptr
			);

			vkFreeMemory(
				vulkanDevice.logicalDevice,
				mesh->getIndexBufferMemory(),
				nullptr
			);
		}
	}

}

MeshModel::~MeshModel()
{
}
