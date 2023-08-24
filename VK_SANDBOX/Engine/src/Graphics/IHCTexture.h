#pragma once
#include "../pch.h"
#include "VKHelpers.h"
#include "IHCDevice.h"
#include "IHCBuffer.h"
#include <stb_image.h>


namespace IHCEngine::Graphics
{
	class IHCTexture
	{
    public:
        IHCTexture(IHCDevice& device, const std::string& filepath);
        ~IHCTexture();

        // no duplication
        IHCTexture(const IHCTexture&) = delete;
        IHCTexture& operator=(const IHCTexture&) = delete;

        VkImageView GetTextureImageView() { return textureImageView; }
        VkSampler GetTextureSampler() { return textureSampler; }

    private:

        void createTextureImage(const std::string filepath);
        // The layout defines how pixels are organized in memory and how it should be accessed
        // Change the layout of an image for Optimization and Correctness
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
        void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
        void createTextureImageView();
        void createTextureSampler();

        IHCDevice& ihcDevice;

        uint32_t mipLevels;
        VkImage textureImage; // pixel data
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView; // Describes how to access the texture image and which part of the image to access
        VkSampler textureSampler; // Used to read data from the texture image in the shaders.
	};

}