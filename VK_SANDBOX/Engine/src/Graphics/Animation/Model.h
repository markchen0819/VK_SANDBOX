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
	// Model
	// storing meshes,
	// materials related to mesh,
	// skinningInfoMap (bone weights and ID)
	// skeletal hierarchy

	class Model
	{
	public:
		Model(const std::string& filepath);
		~Model();

		// Drawing functions
		std::unordered_map<std::string, IHCMesh*> GetMeshes();
		MaterialData GetMaterialForMesh(std::string key);

		// Animation functions
		std::map<std::string, SkinningInfo>& GetSkinningInfoMap() { return skinningInfoMap; }
		int& GetBoneCount() { return boneCounter; }
		SkeletalNodeData& GetRootNodeOfHierarhcy() { return rootNodeOfHierachy;}

	private:

		// Info 
		std::string filepath;
		std::string directory;
		std::string filename;
		int keyID = 0;
		std::string currentKeyStr = ""; // key for following unordered_maps

		// Extracted data (meshes, materials, skinningInfoMap (bone weights and ID), skeletal hierarchy)
		std::unordered_map<std::string, IHCMesh*> meshes;
		std::unordered_map<std::string, MaterialData> meshMaterialMap;
		std::map<std::string, SkinningInfo> skinningInfoMap;
		int boneCounter = 0;
		SkeletalNodeData rootNodeOfHierachy;
		std::map<std::string, SkeletalNodeData*> hierarchyMap; // fast lookup

		// graphics manager & assetManager required to generate texture and meshes
		void loadModel(std::string filepath);
		void processNode(aiNode* node, const aiScene* scene, SkeletalNodeData& root);
		std::pair<std::string, IHCMesh*> processMesh(aiMesh* mesh, const aiScene* scene);
		std::pair<std::string, MaterialData>  processMaterials(aiMesh* mesh, const aiScene* scene);
		std::vector<IHCEngine::Graphics::IHCTexture*> loadTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
		void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
		void storeHierachyInMap(SkeletalNodeData* node);

	};
}

