
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#endif

#ifdef __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA 1
#endif

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11 1
#undef Always
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#ifdef __linux__
#undef Always
#endif

#include <LLGI.CommandList.h>
#include <LLGI.Graphics.h>
#include <LLGI.Platform.h>
#include <LLGI.Texture.h>

#ifdef _WIN32
#pragma comment(lib, "d3dcompiler.lib")
#elif __APPLE__
#endif

// Use a library to use imgui with LLGI
#include <ImGuiPlatform.h>

#ifdef _WIN32
#include <ImGuiPlatformDX12.h>

#elif __APPLE__
#include <ImGuiPlatformMetal.h>
#endif

#ifdef ENABLE_VULKAN
#include <ImGuiPlatformVulkan.h>
#endif

#include <imgui_impl_glfw.h>

class LLGIWindow : public LLGI::Window
{
	GLFWwindow* window_ = nullptr;

public:
	LLGIWindow(GLFWwindow* window) : window_(window) {}

	bool OnNewFrame() override { return glfwWindowShouldClose(window_) == GL_FALSE; }

	void* GetNativePtr(int32_t index) override
	{
#ifdef _WIN32
		if (index == 0)
		{
			return glfwGetWin32Window(window_);
		}

		return (HINSTANCE)GetModuleHandle(0);
#endif

#ifdef __APPLE__
		return glfwGetCocoaWindow(window_);
#endif

#ifdef __linux__
		if (index == 0)
		{
			return glfwGetX11Display();
		}

		return reinterpret_cast<void*>(glfwGetX11Window(window_));
#endif
	}

	LLGI::Vec2I GetWindowSize() const override
	{
		int w, h;
		glfwGetWindowSize(window_, &w, &h);
		return LLGI::Vec2I(w, h);
	}

	LLGI::Vec2I GetFrameBufferSize() const override
	{
		int w, h;
		glfwGetFramebufferSize(window_, &w, &h);
		return LLGI::Vec2I(w, h);
	}
};

static void glfw_error_callback(int error, const char* description) { fprintf(stderr, "Glfw Error %d: %s\n", error, description); }

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	auto window = glfwCreateWindow(1280, 720, "Example imgui", nullptr, nullptr);

	auto llgiwindow = new LLGIWindow(window);

	LLGI::DeviceType deviceType = LLGI::DeviceType::Default;
#ifdef ENABLE_VULKAN
	deviceType = LLGI::DeviceType::Vulkan;
#endif

	LLGI::PlatformParameter platformParam;
	platformParam.Device = deviceType;
	auto platform = LLGI::CreatePlatform(platformParam, llgiwindow);
	auto graphics = platform->CreateGraphics();
	auto sfMemoryPool = graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128);
	auto commandList = graphics->CreateCommandList(sfMemoryPool);

	LLGI::Color8 color;
	color.R = 50;
	color.G = 50;
	color.B = 50;
	color.A = 255;

	// Create simple texture

	LLGI::TextureInitializationParameter textureParam;
	textureParam.Size = LLGI::Vec2I(256, 256);
	auto texture = LLGI::CreateSharedPtr(graphics->CreateTexture(textureParam));

	{
		auto ptr = static_cast<LLGI::Color8*>(texture->Lock());
		if (ptr != nullptr)
		{
			for (size_t y = 0; y < 256; y++)
			{
				for (size_t x = 0; x < 256; x++)
				{
					ptr[x + y * 256].R = static_cast<uint8_t>(x);
					ptr[x + y * 256].G = static_cast<uint8_t>(y);
					ptr[x + y * 256].B = 0;
					ptr[x + y * 256].A = 255;
				}
			}

			texture->Unlock();
		}
	}

	LLGI::RenderTextureInitializationParameter textureParam2;
	textureParam2.Size = LLGI::Vec2I(256, 256);
	auto texture2 = LLGI::CreateSharedPtr(graphics->CreateRenderTexture(textureParam2));

	{
		auto ptr = static_cast<LLGI::Color8*>(texture2->Lock());
		if (ptr != nullptr)
		{
			for (size_t y = 0; y < 256; y++)
			{
				for (size_t x = 0; x < 256; x++)
				{
					ptr[x + y * 256].R = static_cast<uint8_t>(x);
					ptr[x + y * 256].G = static_cast<uint8_t>(y);
					ptr[x + y * 256].B = 0;
					ptr[x + y * 256].A = 255;
				}
			}

			texture2->Unlock();
		}
	}

	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForVulkan(window, true);

	std::shared_ptr<ImguiPlatform> imguiPlatform;

#ifdef ENABLE_VULKAN
	if (deviceType == LLGI::DeviceType::Vulkan)
	{
		imguiPlatform = std::make_shared<ImguiPlatformVulkan>(graphics, platform);
	}
#endif

	if (imguiPlatform == nullptr)
	{
#if defined(_WIN32)
		imguiPlatform = std::make_shared<ImguiPlatformDX12>(graphics);
#elif defined(__APPLE__)
		imguiPlatform = std::make_shared<ImguiPlatformMetal>(graphics);
#endif
	}

	while (glfwWindowShouldClose(window) == GL_FALSE)
	{
		platform->SetWindowSize(llgiwindow->GetWindowSize());

		if (!platform->NewFrame())
			break;

		sfMemoryPool->NewFrame();

		auto renderPass = platform->GetCurrentScreen(color, true);

		imguiPlatform->NewFrame(renderPass);

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Window");

			ImGui::Text("Hello, Altseed");

			{
				auto texturePtr = imguiPlatform->GetTextureIDToRender(texture.get(), commandList);
				if (texturePtr != nullptr)
				{
					ImGui::Image(texturePtr, ImVec2(256, 256));
				}
			}

			{
				auto texturePtr = imguiPlatform->GetTextureIDToRender(texture2.get(), commandList);
				if (texturePtr != nullptr)
				{
					ImGui::Image(texturePtr, ImVec2(256, 256));
				}
			}

			ImGui::End();
		}
		// It need to create a command buffer between NewFrame and Present.
		// Because get current screen returns other values by every frame.
		commandList->Begin();
		commandList->BeginRenderPass(renderPass);

		// imgui

		ImGui::Render();

		imguiPlatform->RenderDrawData(ImGui::GetDrawData(), commandList);

		commandList->EndRenderPass();
		commandList->End();

		graphics->Execute(commandList);

		platform->Present();

		// glfwSwapBuffers(window);
		glfwPollEvents();
	}

	graphics->WaitFinish();

	imguiPlatform.reset();

	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	LLGI::SafeRelease(sfMemoryPool);
	LLGI::SafeRelease(commandList);
	LLGI::SafeRelease(graphics);
	LLGI::SafeRelease(platform);

	delete llgiwindow;

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}
