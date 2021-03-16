
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "TestHelper.h"
#include "thirdparty/stb/stb_image.h"
#include "thirdparty/stb/stb_image_write.h"
#include <functional>
#include <map>
#include <regex>
#include <string>

struct InternalTestHelper
{
	std::string Root;
	bool IsCaptureRequired = false;
	std::map<std::string, std::function<void(LLGI::DeviceType)>> tests;
};

std::shared_ptr<InternalTestHelper> TestHelper::Get()
{
	// HACK for initializing order in some platforms
	static std::shared_ptr<InternalTestHelper> internalTestHelper;

	if (internalTestHelper == nullptr)
	{
		internalTestHelper = std::shared_ptr<InternalTestHelper>(new InternalTestHelper());
	}

	return internalTestHelper;
}

ParsedArgs TestHelper::ParseArg(int argc, char* argv[])
{
	ParsedArgs args;

	bool isVulkanMode = false;
	std::string filter;

	for (int i = 0; i < argc; i++)
	{
		auto v = std::string(argv[i]);

		if (v == "--vulkan")
		{
			isVulkanMode = true;
		}
		else if (v.find("--filter=") == 0)
		{
			args.Filter = v.substr(strlen("--filter="));
		}
	}

#if defined(WIN32) && 1
	args.Device = LLGI::DeviceType::DirectX12;
#elif defined(__APPLE__)
	args.Device = LLGI::DeviceType::Metal;
#else
	args.Device = LLGI::DeviceType::Vulkan;
#endif

	if (isVulkanMode)
	{
		args.Device = LLGI::DeviceType::Vulkan;
	}

	return args;
}

std::vector<uint8_t> TestHelper::CreateDummyTextureData(LLGI::Vec2I size, LLGI::TextureFormatType format)
{
	std::vector<uint8_t> ret;
	ret.resize(LLGI::GetTextureMemorySize(format, size));

	if (format == LLGI::TextureFormatType::R8G8B8A8_UNORM)
	{
		auto data = reinterpret_cast<LLGI::Color8*>(ret.data());

		for (int y = 0; y < size.Y; y++)
		{
			for (int x = 0; x < size.X; x++)
			{
				data[x + y * size.X].R = static_cast<int>(x % 256);
				data[x + y * size.X].G = static_cast<int>(y % 256);
				data[x + y * size.X].B = static_cast<int>((x % 16 > 8 || y % 16 > 8) ? 128 : 0);
				data[x + y * size.X].A = 255;
			}
		}
	}

	if (format == LLGI::TextureFormatType::R32G32B32A32_FLOAT)
	{
		auto data = reinterpret_cast<LLGI::ColorF*>(ret.data());

		for (int y = 0; y < size.Y; y++)
		{
			for (int x = 0; x < size.X; x++)
			{
				data[x + y * size.X].R = x / static_cast<float>(size.X);
				data[x + y * size.X].G = y / static_cast<float>(size.Y);
				data[x + y * size.X].B = (x % 16 > 8 || y % 16 > 8) ? 0.5f : 0.0f;
				data[x + y * size.X].A = 1.0f;
			}
		}
	}

	if (format == LLGI::TextureFormatType::R8_UNORM)
	{
		auto data = reinterpret_cast<uint8_t*>(ret.data());

		for (int y = 0; y < size.Y; y++)
		{
			for (int x = 0; x < size.X; x++)
			{
				data[x + y * size.X] = (x % 16 > 8 || y % 16 > 8) ? 128 : 0;
			}
		}
	}

	return ret;
}

void TestHelper::WriteDummyTexture(LLGI::Texture* texture)
{
	auto dummyData = CreateDummyTextureData(texture->GetSizeAs2D(), texture->GetFormat());

	auto data = texture->Lock();
	memcpy(data, dummyData.data(), dummyData.size());
	texture->Unlock();
}

void TestHelper::WriteDummyTexture(LLGI::Color8* data, LLGI::Vec2I size)
{
	for (int y = 0; y < size.Y; y++)
	{
		for (int x = 0; x < size.X; x++)
		{
			data[x + y * 256].R = static_cast<uint8_t>(x % 256);
			data[x + y * 256].G = static_cast<uint8_t>(y % 256);
			data[x + y * 256].B = static_cast<uint8_t>((x % 16 > 8 || y % 16 > 8) ? 128 : 0);
			data[x + y * 256].A = 255;
		}
	}
}

std::vector<uint8_t> TestHelper::LoadData(const char* path)
{
	std::vector<uint8_t> ret;
	auto path_ = Get()->Root + path;
	return LoadDataWithoutRoot(path_.c_str());
}

std::vector<uint8_t> TestHelper::LoadDataWithoutRoot(const char* path)
{
	std::vector<uint8_t> ret;

#ifdef _WIN32
	FILE* fp = nullptr;
	fopen_s(&fp, path, "rb");

#else
	FILE* fp = fopen(path, "rb");
#endif

	if (fp == nullptr)
	{
		std::cout << "Not found : " << path << std::endl;
		return ret;
	}

	fseek(fp, 0, SEEK_END);
	auto size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	ret.resize(size);
	fread(ret.data(), 1, size, fp);
	fclose(fp);

	return ret;
}

void TestHelper::SetRoot(const char* root)
{
	auto helper = Get();
	helper->Root = root;
}

void TestHelper::CreateRectangle(LLGI::Graphics* graphics,
								 const LLGI::Vec3F& ul,
								 const LLGI::Vec3F& lr,
								 const LLGI::Color8& cul,
								 const LLGI::Color8& clr,
								 std::shared_ptr<LLGI::VertexBuffer>& vb,
								 std::shared_ptr<LLGI::IndexBuffer>& ib)
{
	vb = LLGI::CreateSharedPtr(graphics->CreateVertexBuffer(sizeof(SimpleVertex) * 4));
	ib = LLGI::CreateSharedPtr(graphics->CreateIndexBuffer(2, 6));
	auto vb_buf = (SimpleVertex*)vb->Lock();
	vb_buf[0].Pos = LLGI::Vec3F(ul.X, ul.Y, ul.Z);
	vb_buf[1].Pos = LLGI::Vec3F(lr.X, ul.Y, ul.Z);
	vb_buf[2].Pos = LLGI::Vec3F(lr.X, lr.Y, lr.Z);
	vb_buf[3].Pos = LLGI::Vec3F(ul.X, lr.Y, lr.Z);

	vb_buf[0].UV = LLGI::Vec2F(0.0f, 0.0f);
	vb_buf[1].UV = LLGI::Vec2F(1.0f, 0.0f);
	vb_buf[2].UV = LLGI::Vec2F(1.0f, 1.0f);
	vb_buf[3].UV = LLGI::Vec2F(0.0f, 1.0f);

	vb_buf[0].Color = cul;
	vb_buf[1].Color = LLGI::Color8(cul.R, cul.G, clr.B, cul.A);
	vb_buf[2].Color = clr;
	vb_buf[3].Color = LLGI::Color8(clr.R, clr.G, cul.B, clr.A);

	vb->Unlock();

	auto ib_buf = (uint16_t*)ib->Lock();
	ib_buf[0] = 0;
	ib_buf[1] = 1;
	ib_buf[2] = 2;
	ib_buf[3] = 0;
	ib_buf[4] = 2;
	ib_buf[5] = 3;
	ib->Unlock();
}

void TestHelper::CreateShader(LLGI::Graphics* graphics,
							  LLGI::DeviceType deviceType,
							  const char* vsBinaryPath,
							  const char* psBinaryPath,
							  std::shared_ptr<LLGI::Shader>& vs,
							  std::shared_ptr<LLGI::Shader>& ps)
{
	auto compiler = LLGI::CreateSharedPtr(LLGI::CreateCompiler(deviceType));

	std::vector<LLGI::DataStructure> data_vs;
	std::vector<LLGI::DataStructure> data_ps;

	if (compiler == nullptr)
	{
		auto vsBinaryPath_ = std::string(vsBinaryPath);
		auto psBinaryPath_ = std::string(psBinaryPath);

		// if (deviceType == LLGI::DeviceType::Vulkan)
		{
			vsBinaryPath_ += ".spv";
			psBinaryPath_ += ".spv";
		}

		auto binary_vs = TestHelper::LoadData(vsBinaryPath_.c_str());
		auto binary_ps = TestHelper::LoadData(psBinaryPath_.c_str());

		LLGI::DataStructure d_vs;
		LLGI::DataStructure d_ps;

		d_vs.Data = binary_vs.data();
		d_vs.Size = static_cast<int32_t>(binary_vs.size());
		d_ps.Data = binary_ps.data();
		d_ps.Size = static_cast<int32_t>(binary_ps.size());

		data_vs.push_back(d_vs);
		data_ps.push_back(d_ps);

		vs = LLGI::CreateSharedPtr(graphics->CreateShader(data_vs.data(), static_cast<int32_t>(data_vs.size())));
		ps = LLGI::CreateSharedPtr(graphics->CreateShader(data_ps.data(), static_cast<int32_t>(data_ps.size())));
	}
	else
	{
		LLGI::CompilerResult result_vs;
		LLGI::CompilerResult result_ps;

		auto vsBinaryPath_ = std::string(vsBinaryPath);
		auto psBinaryPath_ = std::string(psBinaryPath);

		auto code_vs = TestHelper::LoadData(vsBinaryPath_.c_str());
		auto code_ps = TestHelper::LoadData(psBinaryPath_.c_str());
		code_vs.push_back(0);
		code_ps.push_back(0);

		compiler->Compile(result_vs, (const char*)code_vs.data(), LLGI::ShaderStageType::Vertex);
		compiler->Compile(result_ps, (const char*)code_ps.data(), LLGI::ShaderStageType::Pixel);

		std::cout << result_vs.Message.c_str() << std::endl;
		std::cout << result_ps.Message.c_str() << std::endl;

		for (auto& b : result_vs.Binary)
		{
			LLGI::DataStructure d;
			d.Data = b.data();
			d.Size = static_cast<int32_t>(b.size());
			data_vs.push_back(d);
		}

		for (auto& b : result_ps.Binary)
		{
			LLGI::DataStructure d;
			d.Data = b.data();
			d.Size = static_cast<int32_t>(b.size());
			data_ps.push_back(d);
		}

		vs = LLGI::CreateSharedPtr(graphics->CreateShader(data_vs.data(), static_cast<int32_t>(data_vs.size())));
		ps = LLGI::CreateSharedPtr(graphics->CreateShader(data_ps.data(), static_cast<int32_t>(data_ps.size())));
	}
}

void TestHelper::Run(const ParsedArgs& args)
{
	auto helper = Get();

	if (args.Filter == "")
	{
		for (auto& f : helper->tests)
		{
			std::cout << "Start : " << f.first << std::endl;
			f.second(args.Device);
		}
	}
	else
	{
		std::basic_regex<char> re(args.Filter);

		for (auto& f : Get()->tests)
		{
			if (!std::regex_match(f.first, re))
				continue;

			std::cout << "Start : " << f.first << std::endl;
			f.second(args.Device);
		}
	}
}

void TestHelper::RegisterTest(const char* name, std::function<void(LLGI::DeviceType)> func)
{
	auto helper = Get();
	helper->tests[name] = func;
}

bool TestHelper::GetIsCaptureRequired() { return Get()->IsCaptureRequired; }

void TestHelper::SetIsCaptureRequired(bool required) { Get()->IsCaptureRequired = required; }

void TestHelper::Dispose()
{
	//	internalTestHelper.reset();
}

Bitmap2D::Bitmap2D(const std::vector<uint8_t>& data, int width, int height, LLGI::TextureFormatType format)
	: data_(data), width_(width), height_(height)
{
	if (format == LLGI::TextureFormatType::B8G8R8A8_UNORM)
	{
		for (int i = 0; i < width_ * height_; ++i)
		{
			auto* b = &data_[i * 4];
			std::swap(b[0], b[2]);
		}
	}
	else if (format == LLGI::TextureFormatType::R8G8B8A8_UNORM || format == LLGI::TextureFormatType::R8G8B8A8_UNORM_SRGB)
	{
	}
	else
	{
		assert(0);
	}
}

Bitmap2D::Color Bitmap2D::GetPixel(int x, int y) const
{
	auto* b = &data_[(x + y * width_) * 4];
	return Bitmap2D::Color{b[0], b[1], b[2], b[3]};
}

void Bitmap2D::Save(const char* filePath) { stbi_write_png(filePath, width_, height_, 4, data_.data(), width_ * 4); }
