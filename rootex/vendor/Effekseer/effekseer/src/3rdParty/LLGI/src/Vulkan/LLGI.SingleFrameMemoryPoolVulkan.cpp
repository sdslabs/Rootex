#include "LLGI.SingleFrameMemoryPoolVulkan.h"
#include "LLGI.ConstantBufferVulkan.h"

namespace LLGI
{

InternalSingleFrameMemoryPoolVulkan::InternalSingleFrameMemoryPoolVulkan() {}

InternalSingleFrameMemoryPoolVulkan ::~InternalSingleFrameMemoryPoolVulkan() {}

bool InternalSingleFrameMemoryPoolVulkan::Initialize(GraphicsVulkan* graphics, int32_t constantBufferPoolSize, int32_t drawingCount)
{
	constantBufferSize_ = (constantBufferPoolSize + 255) & ~255; // buffer size should be multiple of 256

	nativeDevice_ = static_cast<VkDevice>(graphics->GetDevice());

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = constantBufferSize_;
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; // for constant buffer
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	LLGI_VK_CHECK(vkCreateBuffer(nativeDevice_, &bufferInfo, nullptr, &nativeBuffer_));

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(nativeDevice_, nativeBuffer_, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = graphics->GetMemoryTypeIndex(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible);

	LLGI_VK_CHECK(vkAllocateMemory(nativeDevice_, &allocInfo, nullptr, &nativeBufferMemory_));

	LLGI_VK_CHECK(vkBindBufferMemory(nativeDevice_, nativeBuffer_, nativeBufferMemory_, 0));

	return true;
}

void InternalSingleFrameMemoryPoolVulkan::Dispose()
{
	if (nativeBufferMemory_)
	{
		vkFreeMemory(nativeDevice_, nativeBufferMemory_, nullptr);
		nativeBufferMemory_ = VK_NULL_HANDLE;
	}

	if (nativeBuffer_)
	{
		vkDestroyBuffer(nativeDevice_, nativeBuffer_, nullptr);
		nativeBuffer_ = VK_NULL_HANDLE;
	}

	nativeDevice_ = VK_NULL_HANDLE;
}

bool InternalSingleFrameMemoryPoolVulkan::GetConstantBuffer(int32_t size,
															VkBuffer* outResource,
															VkDeviceMemory* deviceMemory,
															int32_t* outOffset)
{
	if (constantBufferOffset_ + size > constantBufferSize_)
		return false;

	*outResource = nativeBuffer_;
	*deviceMemory = nativeBufferMemory_;
	*outOffset = constantBufferOffset_;
	constantBufferOffset_ += size;
	return true;
}

void InternalSingleFrameMemoryPoolVulkan::Reset() { constantBufferOffset_ = 0; }

ConstantBuffer* SingleFrameMemoryPoolVulkan::CreateConstantBufferInternal(int32_t size)
{
	auto obj = new ConstantBufferVulkan();
	if (!obj->InitializeAsShortTime(graphics_, this, size))
	{
		SafeRelease(obj);
		return nullptr;
	}

	return obj;
}

ConstantBuffer* SingleFrameMemoryPoolVulkan::ReinitializeConstantBuffer(ConstantBuffer* cb, int32_t size)
{
	auto obj = static_cast<ConstantBufferVulkan*>(cb);
	if (!obj->InitializeAsShortTime(graphics_, this, size))
	{
		return nullptr;
	}

	return obj;
}

SingleFrameMemoryPoolVulkan::SingleFrameMemoryPoolVulkan(
	GraphicsVulkan* graphics, bool isStrongRef, int32_t swapBufferCount, int32_t constantBufferPoolSize, int32_t drawingCount)
	: SingleFrameMemoryPool(swapBufferCount), graphics_(graphics), isStrongRef_(isStrongRef), currentSwap_(-1), drawingCount_(drawingCount)
{
	if (isStrongRef)
	{
		SafeAddRef(graphics_);
	}

	for (int32_t i = 0; i < swapBufferCount; i++)
	{
		auto memoryPool = std::make_shared<InternalSingleFrameMemoryPoolVulkan>();
		if (!memoryPool->Initialize(graphics, constantBufferPoolSize, drawingCount))
		{
			return;
		}
		memoryPools.push_back(memoryPool);
	}
}

SingleFrameMemoryPoolVulkan ::~SingleFrameMemoryPoolVulkan()
{
	for (auto& pool : memoryPools)
	{
		pool->Dispose();
	}
	memoryPools.clear();

	if (isStrongRef_)
	{
		SafeRelease(graphics_);
	}
}

bool SingleFrameMemoryPoolVulkan::GetConstantBuffer(int32_t size, VkBuffer* outResource, VkDeviceMemory* deviceMemory, int32_t* outOffset)
{
	assert(currentSwap_ >= 0);
	return memoryPools[currentSwap_]->GetConstantBuffer(size, outResource, deviceMemory, outOffset);
}

InternalSingleFrameMemoryPoolVulkan* SingleFrameMemoryPoolVulkan::GetInternal() { return memoryPools[currentSwap_].get(); }

int32_t SingleFrameMemoryPoolVulkan::GetDrawingCount() const { return drawingCount_; }

void SingleFrameMemoryPoolVulkan::NewFrame()
{
	currentSwap_++;
	currentSwap_ %= memoryPools.size();
	memoryPools[currentSwap_]->Reset();
	SingleFrameMemoryPool::NewFrame();
}

} // namespace LLGI
