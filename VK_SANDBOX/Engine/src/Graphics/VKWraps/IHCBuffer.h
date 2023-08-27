#pragma once
#include "VKHelpers.h"
#include "IHCDevice.h"

namespace IHCEngine::Graphics
{
    class IHCDevice;  // Forward declaration, BUT WHY?
	class IHCBuffer
	{
    public:
        IHCBuffer(
            IHCDevice& device,
            VkDeviceSize instanceSize, // size of data structure (ex: vertex)
            uint32_t instanceCount, // number of data structures (vertices)
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkDeviceSize minOffsetAlignment = 1);
        ~IHCBuffer();

        // no duplication
        IHCBuffer(const IHCBuffer&) = delete;
        IHCBuffer& operator=(const IHCBuffer&) = delete;

        // example:  
        // letter (data)
        // mailbox (the buffer) 
        // the mail carrier (the GPU), mostly come and pickup (but might miss)
        // 
        // Mapping & Writing: Putting a letter in your mailbox
        // flush (notify), ensure noticed for pickup

        // Map/ Unmap a memory range of this buffer 
        VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void Unmap();

        // Copying the Data to the mapped buffer (memcpy)
        void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void WriteToIndex(void* data, int index); // portion of the buffer


        // DescriptorInfo: Interface between shaders and buffers
        VkDescriptorBufferInfo GetDescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkDescriptorBufferInfo GetDescriptorInfoForIndex(int index); // portion

        // Non-coherent memory 
        // requires explicit actions to ensure visibility of writes across the host and the device
        // if using  VK_MEMORY_PROPERTY_HOST_CACHED_BIT flag
        // the data writes to the memory might be cached.
        // To ensure that these writes are visible to the device (GPU)
        // you might need to flush the memory 
        VkResult FlushIndex(int index);
        VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult InvalidateIndex(int index);

        VkBuffer GetBuffer() const { return buffer; }
        void* GetMappedMemory() const { return mappedMemory; }
        uint32_t GetInstanceCount() const { return instanceCount; }
        VkDeviceSize GetInstanceSize() const { return instanceSize; }
        VkDeviceSize GetAlignmentSize() const { return instanceSize; }
        VkBufferUsageFlags GetUsageFlags() const { return usageFlags; }
        VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return memoryPropertyFlags; }
        VkDeviceSize GetBufferSize() const { return bufferSize; }

    private:

        VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        IHCDevice& ihcDevice;
        void* mappedMemory = nullptr; // obtain a pointer to buffer memory.
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        VkDeviceSize bufferSize;
        uint32_t instanceCount;
        VkDeviceSize instanceSize;
        VkDeviceSize alignmentSize;
        VkBufferUsageFlags usageFlags;
        VkMemoryPropertyFlags memoryPropertyFlags;
	};

}