
#pragma once

#include "../LLGI.Texture.h"
#include "LLGI.BaseVulkan.h"
#include "LLGI.GraphicsVulkan.h"

namespace LLGI
{

// for Texture2D, RenderTarget, DepthBuffer
class TextureVulkan : public Texture
{
private:
	GraphicsVulkan* graphics_ = nullptr;
	ReferenceObject* owner_ = nullptr;
	vk::Device device_ = nullptr;

	bool isStrongRef_ = false;
	vk::Image image_ = nullptr;
	vk::ImageView view_ = nullptr;
	std::vector<vk::ImageLayout> imageLayouts_;
	vk::DeviceMemory devMem_ = nullptr;
	vk::Format vkTextureFormat_;
	vk::ImageSubresourceRange subresourceRange_;

	Vec2I textureSize;

	int32_t memorySize = 0;
	std::unique_ptr<Buffer> cpuBuf;
	void* data = nullptr;

	bool isExternalResource_ = false;

	void ResetImageLayouts(int32_t count, vk::ImageLayout layout);

public:
	TextureVulkan();
	~TextureVulkan() override;

	bool Initialize(GraphicsVulkan* graphics,
					bool isStrongRef,
					const Vec2I& size,
					vk::Format format,
					int samplingCount,
					int mipmapCount,
					TextureType textureType);

	bool InitializeAsRenderTexture(GraphicsVulkan* graphics, bool isStrongRef, const RenderTextureInitializationParameter& parameter);

	/**
		@brief	initialize as screen
	*/
	bool InitializeAsScreen(const vk::Image& image, const vk::ImageView& imageVew, vk::Format format, const Vec2I& size);

	bool InitializeAsDepthStencil(vk::Device device,
								  vk::PhysicalDevice physicalDevice,
								  const Vec2I& size,
								  vk::Format format,
								  int samplingCount,
								  ReferenceObject* owner);

	bool InitializeAsExternal(vk::Device device, const VulkanImageInfo& info, ReferenceObject* owner);

	void* Lock() override;
	void Unlock() override;
	Vec2I GetSizeAs2D() const override;

	const vk::Image& GetImage() const { return image_; }
	const vk::ImageView& GetView() const { return view_; }

	vk::Format GetVulkanFormat() const { return vkTextureFormat_; }
	int32_t GetMemorySize() const { return memorySize; }

	std::vector<vk::ImageLayout> GetImageLayouts() const;

	vk::ImageSubresourceRange GetSubresourceRange() const { return subresourceRange_; }

	void ChangeImageLayout(const vk::ImageLayout& imageLayout);

	void ChangeImageLayout(int32_t mipLevel, const vk::ImageLayout& imageLayout);

	void ResourceBarrior(vk::CommandBuffer& commandBuffer, const vk::ImageLayout& imageLayout);

	void ResourceBarrior(int32_t mipLevel, vk::CommandBuffer& commandBuffer, const vk::ImageLayout& imageLayout);
};

} // namespace LLGI
