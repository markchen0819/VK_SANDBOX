#include "../../pch.h"
#include "IHCBuffer.h"
#include "IHCDevice.h"

// ensure proper memory alignment for an instance (data structure) on the GPU
// given specific hardware alignment requirements.
VkDeviceSize IHCEngine::Graphics::IHCBuffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
{
    // Easy to understand example:
    // bookshelf (5 inches)
    // book  (5 inches or less) -> fit
    // book  (more than 5 inches ) -> inefficient or error! 
    // if book 5, shelf 4 -> (4*2=8 > 5) -> use 8 
    // 
    // instanceSize: width of the book 
    // minOffsetAlignment: width of the shelf ( The minimum alignment required by the GPU. )
    // aligned -> accessed faster
    // not aligned -> multiple access cycles or could even lead to crashes in some systems
    // 
    // Calculation
    // ensure that when the data is divided by the alignment requirement, there's no remainder
    // 
    // (instanceSize + minOffsetAlignment - 1)
    // ensures that we're overshooting the alignment
    //
    // ~(minOffsetAlignment - 1)
    // bitwise negation, For many powers of two alignments, this results in a bitmask that ensures the value is a multiple of the alignment
    //
    // bitwise AND 
    // vershoot and ensures it's a multiple of the alignment by zeroing out the bits that are less significant than the alignment

    if (minOffsetAlignment > 0) 
    {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

IHCEngine::Graphics::IHCBuffer::IHCBuffer(IHCDevice& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
    : ihcDevice{ device },
    instanceSize{ instanceSize },
    instanceCount{ instanceCount },
    usageFlags{ usageFlags },
    memoryPropertyFlags{ memoryPropertyFlags } 
{
	alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
	bufferSize = alignmentSize * instanceCount;
	device.CreateBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
}


IHCEngine::Graphics::IHCBuffer::~IHCBuffer()
{
    Unmap();
    vkDestroyBuffer(ihcDevice.GetDevice(), buffer, nullptr);
    vkFreeMemory(ihcDevice.GetDevice(), memory, nullptr);
}

#pragma region Map/Unmap a memory range of this buffer
VkResult IHCEngine::Graphics::IHCBuffer::Map(VkDeviceSize size, VkDeviceSize offset)
{
    assert(buffer && memory && "Called map on buffer before create");
    return vkMapMemory(ihcDevice.GetDevice(), memory, offset, size, 0, &mappedMemory);
}

void IHCEngine::Graphics::IHCBuffer::Unmap()
{
    if (mappedMemory)
    {
        vkUnmapMemory(ihcDevice.GetDevice(), memory);
        mappedMemory = nullptr;
    }
}
#pragma endregion

#pragma region Write to buffer (memcpy)
// memcpy, like CPU to Staging Buffer
void IHCEngine::Graphics::IHCBuffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
{
    // This function replaces:
    assert(mappedMemory && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) 
    {
        memcpy(mappedMemory, data, bufferSize);
    }
    else 
    {
        char* memOffset = (char*)mappedMemory;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}
void IHCEngine::Graphics::IHCBuffer::WriteToIndex(void* data, int index)
{
    WriteToBuffer(data, instanceSize, index * alignmentSize);
}
#pragma endregion

#pragma region DescriptorInfo: Interface between shaders and buffers
VkDescriptorBufferInfo IHCEngine::Graphics::IHCBuffer::GetDescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
{
    return VkDescriptorBufferInfo{
     buffer,
     offset,
     size,
    };
}
VkDescriptorBufferInfo IHCEngine::Graphics::IHCBuffer::GetDescriptorInfoForIndex(int index)
{
    return GetDescriptorInfo(alignmentSize, index * alignmentSize);
}
#pragma endregion

#pragma region Non-coherent memory
VkResult IHCEngine::Graphics::IHCBuffer::Flush(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(ihcDevice.GetDevice(), 1, &mappedRange);
}
VkResult IHCEngine::Graphics::IHCBuffer::FlushIndex(int index)
{
    return Flush(alignmentSize, index * alignmentSize);
}
VkResult IHCEngine::Graphics::IHCBuffer::Invalidate(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(ihcDevice.GetDevice(), 1, &mappedRange);
    // make sure GPU changes reflect on CPU
}
VkResult IHCEngine::Graphics::IHCBuffer::InvalidateIndex(int index)
{
    return Invalidate(alignmentSize, index * alignmentSize);
}
#pragma endregion