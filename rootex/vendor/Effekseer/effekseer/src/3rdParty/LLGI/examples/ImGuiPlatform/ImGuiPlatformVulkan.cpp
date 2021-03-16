#include "ImGuiPlatformVulkan.h"
#include <Vulkan/LLGI.TextureVulkan.h>

ImguiPlatformVulkan::ImguiPlatformVulkan(LLGI::Graphics* g, LLGI::Platform* p)
	: g_(static_cast<LLGI::GraphicsVulkan*>(g)), p_(static_cast<LLGI::PlatformVulkan*>(p))
{
	{
		VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
											 {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
											 {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
											 {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
											 {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
											 {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
											 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
											 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
											 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
											 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
											 {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		vkCreateDescriptorPool(g_->GetDevice(), &pool_info, nullptr, &descriptorPool_);
		assert(descriptorPool_ != nullptr);
	}

	LLGI::RenderPassPipelineStateKey psk;
	psk.IsPresent = true;
	psk.DepthFormat = LLGI::TextureFormatType::D24S8;
	psk.RenderTargetFormats.resize(1);
	psk.RenderTargetFormats.at(0) = LLGI::TextureFormatType::B8G8R8A8_UNORM;

	ps_ = static_cast<LLGI::RenderPassPipelineStateVulkan*>(g_->CreateRenderPassPipelineState(psk));

	ImGui_ImplVulkan_InitInfo info{};
	info.Instance = p_->GetInstance();
	info.PhysicalDevice = p_->GetPhysicalDevice();
	info.Device = g_->GetDevice();
	info.QueueFamily = p_->GetQueueFamilyIndex();
	info.Queue = p_->GetQueue();
	info.PipelineCache = p_->GetPipelineCache();
	info.DescriptorPool = descriptorPool_;
	info.MinImageCount = p_->GetSwapBufferCountMin();
	info.ImageCount = p_->GetSwapBufferCount();
	info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	info.Allocator = nullptr;

	ImGui_ImplVulkan_Init(&info, ps_->GetRenderPass());

	// update fonts
	auto sfm = g_->CreateSingleFrameMemoryPool(1024 * 128, 128);
	auto cl = static_cast<LLGI::CommandListVulkan*>(g_->CreateCommandList(sfm));

	sfm->NewFrame();
	cl->Begin();

	ImGui_ImplVulkan_CreateFontsTexture(cl->GetCommandBuffer());

	cl->End();
	g_->Execute(cl);
	g_->WaitFinish();

	LLGI::SafeRelease(sfm);
	LLGI::SafeRelease(cl);

	ImGui_ImplVulkan_DestroyFontUploadObjects();
	/*
		// Upload Fonts
{
	// Use any command queue
	VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
	VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

	err = vkResetCommandPool(g_Device, command_pool, 0);
	check_vk_result(err);
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	err = vkBeginCommandBuffer(command_buffer, &begin_info);
	check_vk_result(err);

	ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

	VkSubmitInfo end_info = {};
	end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	end_info.commandBufferCount = 1;
	end_info.pCommandBuffers = &command_buffer;
	err = vkEndCommandBuffer(command_buffer);
	check_vk_result(err);
	err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
	check_vk_result(err);

	err = vkDeviceWaitIdle(g_Device);
	check_vk_result(err);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}
	*/

	vk::SamplerCreateInfo samplerInfo;
	samplerInfo.magFilter = vk::Filter::eLinear;
	samplerInfo.minFilter = vk::Filter::eLinear;
	samplerInfo.anisotropyEnable = false;
	samplerInfo.maxAnisotropy = 1;
	samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
	samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
	samplerInfo.unnormalizedCoordinates = false;
	samplerInfo.compareEnable = false;
	samplerInfo.compareOp = vk::CompareOp::eAlways;
	samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	defaultSampler_ = g_->GetDevice().createSampler(samplerInfo);
}

ImguiPlatformVulkan::~ImguiPlatformVulkan()
{
	for (auto& it : textures_)
	{
		VkDescriptorSet ds = (VkDescriptorSet)it.second.id;
		vkFreeDescriptorSets(g_->GetDevice(), descriptorPool_, 1, &ds);
	}

	vkDestroyDescriptorPool(g_->GetDevice(), descriptorPool_, nullptr);

	if (defaultSampler_)
	{
		g_->GetDevice().destroySampler(defaultSampler_);
	}

	ImGui_ImplVulkan_Shutdown();
	LLGI::SafeRelease(ps_);
}

void ImguiPlatformVulkan::NewFrame(LLGI::RenderPass* renderPass)
{
	for (auto it = textures_.begin(); it != textures_.end();)
	{
		if (it->second.life == 0)
		{
			VkDescriptorSet ds = (VkDescriptorSet)it->second.id;
			vkFreeDescriptorSets(g_->GetDevice(), descriptorPool_, 1, &ds);
			it = textures_.erase(it);
		}
		else
		{
			it->second.life--;
			++it;
		}
	}

	ImGui_ImplVulkan_NewFrame();
}

ImTextureID ImguiPlatformVulkan::GetTextureIDToRender(LLGI::Texture* texture, LLGI::CommandList* commandList)
{
	auto it = textures_.find(texture);
	if (it != textures_.end())
	{
		it->second.life = 10;
		return it->second.id;
	}

	auto textureVulkan = static_cast<LLGI::TextureVulkan*>(texture);
	auto id = ImGui_ImplVulkan_AddTexture(defaultSampler_, textureVulkan->GetView(), (VkImageLayout)textureVulkan->GetImageLayouts()[0]);

	TextureHolder th;
	th.texture = LLGI::CreateSharedPtr(texture, true);
	th.life = 10;
	th.id = id;

	textures_[texture] = th;
	return id;
}

void ImguiPlatformVulkan::CreateFont()
{
	auto sfm = g_->CreateSingleFrameMemoryPool(1024 * 128, 128);
	auto cl = static_cast<LLGI::CommandListVulkan*>(g_->CreateCommandList(sfm));

	sfm->NewFrame();
	cl->Begin();

	ImGui_ImplVulkan_CreateFontsTexture(cl->GetCommandBuffer());

	cl->End();
	g_->Execute(cl);
	g_->WaitFinish();

	LLGI::SafeRelease(sfm);
	LLGI::SafeRelease(cl);

	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void ImguiPlatformVulkan::DisposeFont()
{
	ImGuiIO& io = ImGui::GetIO();
	VkDescriptorSet ds = (VkDescriptorSet)io.Fonts->TexID;
	vkFreeDescriptorSets(g_->GetDevice(), descriptorPool_, 1, &ds);
	io.Fonts->TexID = nullptr;
}
