#include "../../pch.h"
#include "Model.h"
#include <assimp/postprocess.h>

#include "AssimpGLMHelpers.h"
#include "../VKWraps/VKHelpers.h"
#include "../../Engine/src/Core/Locator/GraphicsManagerLocator.h"
#include "../../Engine/src/Core/Locator/AssetManagerLocator.h"



IHCEngine::Graphics::Model::Model(const std::string& filepath)
	:filepath{filepath}
{
	loadModel(filepath);
}

IHCEngine::Graphics::Model::~Model()
{
    auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

    for (const auto& pair : meshes)
    {
        assetManager->GetMeshRepository().RemoveAsset(pair.first);
    }
    for (const auto& pair : meshMaterialMap)
    {
        for (size_t i = 0; i < pair.second.diffuseMaps.size(); ++i)
        {
            assetManager->GetTextureRepository().RemoveAsset(pair.second.diffuseMaps[i]->GetName());
        }
        for (size_t i = 0; i < pair.second.specularMaps.size(); ++i)
        {
            assetManager->GetTextureRepository().RemoveAsset(pair.second.specularMaps[i]->GetName());
        }
        for (size_t i = 0; i < pair.second.normalMaps.size(); ++i)
        {
            assetManager->GetTextureRepository().RemoveAsset(pair.second.normalMaps[i]->GetName());
        }
        for (size_t i = 0; i < pair.second.heightMaps.size(); ++i)
        {
            assetManager->GetTextureRepository().RemoveAsset(pair.second.heightMaps[i]->GetName());
        }
    }
}

std::unordered_map<std::string, IHCEngine::Graphics::IHCMesh*> IHCEngine::Graphics::Model::GetMeshes()
{
    return meshes;
}

IHCEngine::Graphics::MaterialData IHCEngine::Graphics::Model::GetMaterialForMesh(std::string key)
{
    return meshMaterialMap[key];
}

void IHCEngine::Graphics::Model::loadModel(std::string filepath)
{
    Assimp::Importer importer;
    const aiScene* ai_scene = importer.ReadFile(filepath, 
        aiProcess_Triangulate 
        | aiProcess_GenSmoothNormals 
        | aiProcess_FlipUVs
        | aiProcess_CalcTangentSpace);
    // aiProcess_Triangulate: convert all to triangles
    // aiProcess_FlipUVs: flip on y-axis
    // aiProcess_GenNormals: create normal vectors
    // aiProcess_SplitLargeMeshes: split if mesh is too large
    // aiProcess_OptimizeMeshes: combine for reducing draw calls
    if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    directory = filepath.substr(0, filepath.find_last_of('/'));
    filename = filepath.substr(filepath.find_last_of('/') + 1);
    std::cout << "directory:" << directory <<std::endl;
    std::cout << " filename:" << filename << std::endl;
    std::cout << "mNumMeshes:" << ai_scene->mNumMeshes << std::endl;
    std::cout << "aiScene Info:"<<std::endl;
    std::cout << "mNumMeshes:" << ai_scene->mNumMeshes <<std::endl;
    std::cout << "mNumMaterials:" << ai_scene->mNumMaterials << std::endl;
    std::cout << "mNumTextures:" << ai_scene->mNumTextures << std::endl;
    std::cout << "============" << std::endl;
    // process ASSIMP's root node recursively
    processNode(ai_scene->mRootNode, ai_scene);

    std::cout <<"============"<< std::endl;
    for (const auto& pair : meshMaterialMap)
    {
        std::cout << "- Key: " << pair.first << std::endl;
        std::cout << "MaterialName: "<<pair.second.name << std::endl;
        std::cout << "Textures: " <<std::endl;
        for (size_t i = 0; i < pair.second.diffuseMaps.size(); ++i)
        {
            std::cout << pair.second.diffuseMaps[i]->GetName() << std::endl;
        }
        for (size_t i = 0; i < pair.second.specularMaps.size(); ++i)
        {
            std::cout << pair.second.specularMaps[i]->GetName() << std::endl;
        }
        for (size_t i = 0; i < pair.second.normalMaps.size(); ++i)
        {
            std::cout << pair.second.normalMaps[i]->GetName() << std::endl;
        }
        for (size_t i = 0; i < pair.second.heightMaps.size(); ++i)
        {
            std::cout << pair.second.heightMaps[i]->GetName() << std::endl;
        }
        std::cout << std::endl;
    }

}

void IHCEngine::Graphics::Model::processNode(aiNode* node, const aiScene* scene)
{
    // process current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.insert(processMesh(mesh, scene));
        meshMaterialMap.insert(processMaterials(mesh, scene));
        extractBoneWeightForVertices(meshes[currentKeyStr]->GetVertices(), mesh, scene);
        meshes[currentKeyStr]->UpdateVertices(); // must need as boneinfo must write into buffer
    }
    // recursively process children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

std::pair<std::string, IHCEngine::Graphics::IHCMesh*> IHCEngine::Graphics::Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    IHCEngine::Graphics::IHCMesh::Builder meshBuilder;
    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;

        // position
        vertex.position = AssimpGLMHelpers::GetGLMVec3(mesh->mVertices[i]);
        // normal
        if (mesh->HasNormals())
        {
            vertex.normal = AssimpGLMHelpers::GetGLMVec3(mesh->mNormals[i]);
        }
        // color
        if (mesh->HasVertexColors(i))
        {
            vector.x = mesh->mColors[0][i].r;
            vector.y = mesh->mColors[0][i].g;
            vector.z = mesh->mColors[0][i].b;
            //vector.w = mesh->mColors[0][i].a;
            vertex.color = vector;
        }
        else
        {
            vertex.color = glm::vec3(1.0f);
        }
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D diffuseColor; //Testing, need removal
        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor))
        {
            vertex.color = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // Assimp allows a model to have up to
            // 8 different texture coordinates per vertex
            // We thus make the assumption that we won't 
            // use models where a vertex can have multiple
            // texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        }
        else
        {
            vertex.uv = glm::vec2(0.0f, 0.0f);
        }
        // Bones
        for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
        {
            vertex.boneIDs[i] = -1;
            vertex.boneWeights[i] = 0.0f;
        }
        meshBuilder.vertices.push_back(vertex);
    }
    // walk through faces and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            meshBuilder.indices.push_back(face.mIndices[j]);
        }
    }
    // Create IHCMesh, store it in asset manager, store ptrs in Model class for reference
    auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
    std::string meshIDStr = filename + "_mesh_" + std::to_string(keyID);
    currentKeyStr = meshIDStr;
    IHCMesh* meshPtr = graphicsManager->GetGraphicsAssetCreator().CreateMesh(currentKeyStr, meshBuilder);
    ++keyID;

    std::cout << meshIDStr << ": vertices: " << meshBuilder.vertices.size();
    std::cout << ", indices: " << meshBuilder.indices.size() << std::endl;
    return { currentKeyStr , meshPtr };
}

std::pair<std::string, IHCEngine::Graphics::MaterialData> IHCEngine::Graphics::Model::processMaterials(aiMesh* mesh, const aiScene* scene)
{
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    MaterialData materialData;
    aiColor3D diffuseColor;
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor))
    {
        materialData.diffuse = ::glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    }
    materialData.name = material->GetName().C_Str();
    std::vector<IHCTexture*> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    materialData.diffuseMaps = diffuseMaps;
    std::vector<IHCTexture*>  specularMaps = loadTextures(material, aiTextureType_SPECULAR, "texture_specular");
    materialData.specularMaps = specularMaps;
    std::vector<IHCTexture*> normalMaps = loadTextures(material, aiTextureType_HEIGHT, "texture_normal");
    materialData.normalMaps = normalMaps;
    std::vector<IHCTexture*> heightMaps = loadTextures(material, aiTextureType_AMBIENT, "texture_height");
    materialData.heightMaps = heightMaps;

    return {currentKeyStr, materialData};
}

std::vector<IHCEngine::Graphics::IHCTexture*> IHCEngine::Graphics::Model::loadTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<IHCEngine::Graphics::IHCTexture*> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {

        auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
        auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

        aiString str;
        mat->GetTexture(type, i, &str);

        // texture was loaded before
        if(assetManager->GetTextureRepository().HasAsset(str.C_Str()))
        {
            break;
        }
        // texture hasn't been loaded already, load it
        IHCTexture* texturePtr = graphicsManager->GetGraphicsAssetCreator().CreateTexture(str.C_Str(), directory+"/"+str.C_Str());
        textures.push_back(texturePtr);
    }
    return textures;
}

void IHCEngine::Graphics::Model::extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    // Check all bones for the mesh
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        // Find bone from our map or add new bone
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCounter;
            newBoneInfo.offsetMatrix = 
                AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCounter;
            ++boneCounter;
        }
        else
        {
            boneID = boneInfoMap[boneName].id;
        }
        assert(boneID != -1);

        // Get weights from a bone and add data to a vertex
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());

            // if vertex hasn't assign bone weight then assign
            for (int i = 0; i < MAX_BONE_WEIGHTS; ++i)
            {
                if (vertices[vertexId].boneIDs[i] < 0)
                {
                    vertices[vertexId].boneWeights[i] = weight;
                    vertices[vertexId].boneIDs[i] = boneID;
                    break;
                }
            }
        }
    }
}
