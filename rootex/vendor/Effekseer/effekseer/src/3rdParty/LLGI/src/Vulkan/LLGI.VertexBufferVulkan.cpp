#include "LLGI.VertexBufferVulkan.h"

namespace LLGI
{

bool VertexBufferVulkan::Initialize(GraphicsVulkan* graphics, int32_t size)
{

	SafeAddRef(graphics);
	graphics_ = CreateSharedPtr(graphics);

	cpuBuf = std::unique_ptr<Buffer>(new Buffer(graphics));
	gpuBuf = std::unique_ptr<Buffer>(new Buffer(graphics));

	// create a buffer on cpu
	{
		vk::BufferCreateInfo vertexBufferInfo;
		vertexBufferInfo.size = size;
		vertexBufferInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
		vk::Buffer buffer = graphics_->GetDevice().createBuffer(vertexBufferInfo);

		vk::MemoryRequirements memReqs = graphics_->GetDevice().getBufferMemoryRequirements(buffer);
		vk::MemoryAllocateInfo memAlloc;
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = graphics_->GetMemoryTypeIndex(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible);
		vk::DeviceMemory devMem = graphics_->GetDevice().allocateMemory(memAlloc);
		graphics_->GetDevice().bindBufferMemory(buffer, devMem, 0);

		cpuBuf->Attach(buffer, devMem);
	}

	// create a buffer on gpu
	{
		vk::BufferCreateInfo vertexBufferInfo;
		vertexBufferInfo.size = size;
		vertexBufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
		vk::Buffer buffer = graphics_->GetDevice().createBuffer(vertexBufferInfo);

		vk::MemoryRequirements memReqs = graphics_->GetDevice().getBufferMemoryRequirements(buffer);
		vk::MemoryAllocateInfo memAlloc;
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = graphics_->GetMemoryTypeIndex(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
		vk::DeviceMemory devMem = graphics_->GetDevice().allocateMemory(memAlloc);
		graphics_->GetDevice().bindBufferMemory(buffer, devMem, 0);

		gpuBuf->Attach(buffer, devMem);
	}

	memSize = size;

	return true;
}

bool VertexBufferVulkan::InitializeAsShortTime(SingleFrameMemoryPoolVulkan* memoryPool, int32_t size) { throw "Not implemented."; }

VertexBufferVulkan::VertexBufferVulkan() {}

VertexBufferVulkan ::~VertexBufferVulkan() {}

void* VertexBufferVulkan::Lock()
{
	data = graphics_->GetDevice().mapMemory(cpuBuf->devMem(), 0, memSize, vk::MemoryMapFlags());
	return data;
}

void* VertexBufferVulkan::Lock(int32_t offset, int32_t size)
{
	data = graphics_->GetDevice().mapMemory(cpuBuf->devMem(), offset, size, vk::MemoryMapFlags());
	return data;
}

void VertexBufferVulkan::Unlock()
{
	isMemoryDirtied_ = true;
	graphics_->GetDevice().unmapMemory(cpuBuf->devMem());

	VkCommandBuffer commandBuffer = graphics_->BeginSingleTimeCommands();
	vk::CommandBuffer commandBufferCpp = static_cast<vk::CommandBuffer>(commandBuffer);

	SendMemoryToGPU(commandBufferCpp);

	graphics_->EndSingleTimeCommands(commandBuffer);
}

void VertexBufferVulkan::SendMemoryToGPU(vk::CommandBuffer& commandBuffer)
{
	if (!isMemoryDirtied_)
	{
		return;
	}

	vk::BufferCopy copyRegion;
	copyRegion.size = memSize;
	commandBuffer.copyBuffer(cpuBuf->buffer(), gpuBuf->buffer(), copyRegion);

	isMemoryDirtied_ = false;
}

int32_t VertexBufferVulkan::GetSize() { return memSize; }

} // namespace LLGI
