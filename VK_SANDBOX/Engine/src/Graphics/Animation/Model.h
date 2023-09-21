#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <map>
#include "MaterialData.h"
#include "AnimationInfo.h"

struct Vertex;

namespace IHCEngine::Graphics
{
	class IHCMesh;
	class IHCTexture;
}

namespace IHCEngine::Graphics
{
	class Model
	{
	public:
		Model(const std::string& filepath);
		~Model();

		std::unordered_map<std::string, IHCMesh*> GetMeshes();
		MaterialData GetMaterialForMesh(std::string key);

		std::map<std::string, BoneInfo>& GetBoneInfoMap() { return boneInfoMap; }
		int& GetBoneCount() { return boneCounter; }
		AssimpNodeData& GetRootNodeOfHierarhcy() { return rootNodeOfHierachy;}
	private:

		std::string filepath;
		std::string directory;
		std::string filename;

		int keyID = 0;
		std::string currentKeyStr = ""; // key for following unordered_maps
		std::unordered_map<std::string, IHCMesh*> meshes;
		std::unordered_map<std::string, MaterialData> meshMaterialMap;
		std::map<std::string, BoneInfo> boneInfoMap;
		int boneCounter = 0;
		AssimpNodeData rootNodeOfHierachy;

		// graphics manager & assetManager required to generate texture and meshes
		void loadModel(std::string filepath);
		void processNode(aiNode* node, const aiScene* scene, AssimpNodeData& root);
		std::pair<std::string, IHCMesh*> processMesh(aiMesh* mesh, const aiScene* scene);
		std::pair<std::string, MaterialData>  processMaterials(aiMesh* mesh, const aiScene* scene);
		std::vector<IHCEngine::Graphics::IHCTexture*> loadTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
		void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
	};
}

