#include "ShaderTranspilerCore.h"

#if defined(ENABLE_GLSLANG_WITHOUT_INSTALL)
#include <SPIRV/GlslangToSpv.h>
#else
#include <glslang/SPIRV/GlslangToSpv.h>
#endif

#include <glslang/Include/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>

#include <functional>

#if(ENABLE_SPIRVCROSS_WITHOUT_INSTALL)
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>
#include <spirv_hlsl.hpp>
#include <spirv_msl.hpp>
#include <spirv_reflect.hpp>
#else
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_hlsl.hpp>
#include <spirv_cross/spirv_msl.hpp>
#include <spirv_cross/spirv_reflect.hpp>
#endif

#include "ResourceLimits.h"

namespace LLGI
{

namespace
{
std::string Replace(std::string target, std::string from_, std::string to_)
{
	std::string::size_type Pos(target.find(from_));

	while (Pos != std::string::npos)
	{
		target.replace(Pos, from_.length(), to_);
		Pos = target.find(from_, Pos + to_.length());
	}

	return target;
}

// https://stackoverflow.com/questions/8518743/get-directory-from-file-path-c/14631366
std::string dirnameOf(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/");
	return (std::string::npos == pos) ? "" : fname.substr(0, pos);
}
} // namespace

// Based on https://github.com/KhronosGroup/glslang/blob/master/StandAlone/DirStackFileIncluder.h

// Default include class for normal include convention of search backward
// through the stack of active include paths (for nested includes).
// Can be overridden to customize.
class DirStackFileIncluder : public glslang::TShader::Includer
{
public:
	DirStackFileIncluder(const std::function<std::vector<std::uint8_t>(std::string)>& onLoad)
		: externalLocalDirectoryCount(0), onLoad_(onLoad)
	{
	}

	virtual IncludeResult* includeLocal(const char* headerName, const char* includerName, size_t inclusionDepth) override
	{
		return readLocalPath(headerName, includerName, static_cast<int>(inclusionDepth));
	}

	virtual IncludeResult* includeSystem(const char* headerName, const char* /*includerName*/, size_t /*inclusionDepth*/) override
	{
		return readSystemPath(headerName);
	}

	// Externally set directories. E.g., from a command-line -I<dir>.
	//  - Most-recently pushed are checked first.
	//  - All these are checked after the parse-time stack of local directories
	//    is checked.
	//  - This only applies to the "local" form of #include.
	//  - Makes its own copy of the path.
	virtual void pushExternalLocalDirectory(const std::string& dir)
	{
		directoryStack.push_back(dir);
		externalLocalDirectoryCount = static_cast<int>(directoryStack.size());
	}

	virtual void releaseInclude(IncludeResult* result) override
	{
		if (result != nullptr)
		{
			delete[] static_cast<tUserDataElement*>(result->userData);
			delete result;
		}
	}

	virtual ~DirStackFileIncluder() override {}

protected:
	typedef char tUserDataElement;
	std::vector<std::string> directoryStack;
	int externalLocalDirectoryCount;
	std::function<std::vector<std::uint8_t>(std::string)> onLoad_;

	// Search for a valid "local" path based on combining the stack of include
	// directories and the nominal name of the header.
	virtual IncludeResult* readLocalPath(const char* headerName, const char* includerName, int depth)
	{
		// Discard popped include directories, and
		// initialize when at parse-time first level.
		directoryStack.resize(depth + externalLocalDirectoryCount);
		if (depth == 1)
			directoryStack.back() = getDirectory(includerName);

		// Find a directory that works, using a reverse search of the include stack.
		for (auto it = directoryStack.rbegin(); it != directoryStack.rend(); ++it)
		{
			std::string path = *it + '/' + headerName;
			std::replace(path.begin(), path.end(), '\\', '/');

			auto file = onLoad_(path);

			if (file.size() > 0)
			{
				directoryStack.push_back(getDirectory(path));

				char* content = new tUserDataElement[file.size()];
				memcpy(content, file.data(), file.size());
				return new IncludeResult(path, content, file.size(), content);
			}
		}

		return nullptr;
	}

	// Search for a valid <system> path.
	// Not implemented yet; returning nullptr signals failure to find.
	virtual IncludeResult* readSystemPath(const char* /*headerName*/) const { return nullptr; }

	// If no path markers, return current working directory.
	// Otherwise, strip file name and return path leading up to it.
	virtual std::string getDirectory(const std::string path) const
	{
		size_t last = path.find_last_of("/\\");
		return last == std::string::npos ? "." : path.substr(0, last);
	}
};

namespace
{
EShLanguage GetGlslangShaderStage(ShaderStageType type)
{
	if (type == ShaderStageType::Vertex)
		return EShLanguage::EShLangVertex;
	if (type == ShaderStageType::Pixel)
		return EShLanguage::EShLangFragment;
	throw std::string("Unimplemented ShaderStage");
}
} // namespace

SPIRV::SPIRV(const std::vector<uint32_t>& data, ShaderStageType shaderStage) : data_(data), shaderStage_(shaderStage) {}

SPIRV::SPIRV(const std::string& error) : error_(error), shaderStage_{} {}

ShaderStageType SPIRV::GetStage() const { return shaderStage_; }

const std::vector<uint32_t>& SPIRV::GetData() const { return data_; }

bool SPIRVTranspiler::Transpile(const std::shared_ptr<SPIRV>& spirv) { return false; }

std::string SPIRVTranspiler::GetErrorCode() const { return errorCode_; }

std::string SPIRVTranspiler::GetCode() const { return code_; }

SPIRVToHLSLTranspiler::SPIRVToHLSLTranspiler(int32_t shaderModel, bool isDX12) : shaderModel_(shaderModel), isDX12_(isDX12) {}

bool SPIRVToHLSLTranspiler::Transpile(const std::shared_ptr<SPIRV>& spirv)
{
	spirv_cross::CompilerHLSL compiler(spirv->GetData());

	std::vector<std::pair<std::string, int>> remapping;

	if (shaderModel_ <= 30 && !isDX12_)
	{
		compiler.build_combined_image_samplers();

		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (auto& remap : compiler.get_combined_image_samplers())
		{
			auto name = spirv_cross::join("Sampler_", compiler.get_name(remap.sampler_id));
			compiler.set_name(remap.combined_id, name);

			auto location = compiler.get_decoration(remap.sampler_id, spv::DecorationBinding);

			remapping.push_back(std::make_pair(name, location));
		}
	}

	if (isDX12_)
	{
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (auto& resource : resources.uniform_buffers)
		{
			if (spirv->GetStage() == ShaderStageType::Vertex)
			{

				compiler.set_decoration(resource.id, spv::DecorationBinding, 0);
			}
			else if (spirv->GetStage() == ShaderStageType::Pixel)
			{

				compiler.set_decoration(resource.id, spv::DecorationBinding, 1);
			}
		}
	}
	else
	{
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (auto& resource : resources.uniform_buffers)
		{
			if (spirv->GetStage() == ShaderStageType::Vertex)
			{

				compiler.set_decoration(resource.id, spv::DecorationBinding, 0);
			}
			else if (spirv->GetStage() == ShaderStageType::Pixel)
			{

				compiler.set_decoration(resource.id, spv::DecorationBinding, 0);
			}
		}
	}

	spirv_cross::CompilerGLSL::Options options;
	options.separate_shader_objects = true;
	compiler.set_common_options(options);

	spirv_cross::CompilerHLSL::Options targetOptions;
	targetOptions.shader_model = shaderModel_;
	compiler.set_hlsl_options(targetOptions);

	code_ = compiler.compile();

	if (shaderModel_ <= 30)
	{
		for (auto nr : remapping)
		{
			auto src = "uniform sampler2D " + nr.first;
			auto dst = src + " : register(s" + std::to_string(nr.second) + ")";
			code_ = Replace(code_, src, dst);
		}

		code_ = Replace(code_, "packoffset", "register");
	}

	return true;
}

bool SPIRVToMSLTranspiler::Transpile(const std::shared_ptr<SPIRV>& spirv)
{

	spirv_cross::CompilerMSL compiler(spirv->GetData());

	std::vector<std::pair<std::string, int>> remapping_texture;
	std::vector<std::pair<std::string, int>> remapping_sampler;

	{
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (const auto& image : resources.separate_images)
		{
			auto name = image.name;
			auto location = compiler.get_decoration(image.id, spv::DecorationBinding);
			remapping_texture.push_back(std::make_pair(name, location));
		}

		for (const auto& sampler : resources.separate_samplers)
		{
			auto name = sampler.name;
			auto location = compiler.get_decoration(sampler.id, spv::DecorationBinding);
			remapping_sampler.push_back(std::make_pair(name, location));
		}
	}

	spirv_cross::CompilerGLSL::Options options;
	compiler.set_common_options(options);

	spirv_cross::CompilerMSL::Options targetOptions;
	compiler.set_msl_options(targetOptions);

	code_ = compiler.compile();

	int32_t ind = 0;
	for (auto nr : remapping_texture)
	{
		auto src = nr.first + " [[texture(" + std::to_string(ind) + ")]]";
		auto dst = nr.first + " [[texture(" + std::to_string(nr.second) + ")]]";
		code_ = Replace(code_, src, dst);
		ind++;
	}

	ind = 0;
	for (auto nr : remapping_sampler)
	{
		auto src = nr.first + " [[sampler(" + std::to_string(ind) + ")]]";
		auto dst = nr.first + " [[sampler(" + std::to_string(nr.second) + ")]]";
		code_ = Replace(code_, src, dst);
		ind++;
	}

	return true;
}

bool SPIRVToGLSLTranspiler::Transpile(const std::shared_ptr<SPIRV>& spirv)
{
	spirv_cross::CompilerGLSL compiler(spirv->GetData());

	// to combine a sampler and a texture
	compiler.build_combined_image_samplers();

	spirv_cross::ShaderResources resources = compiler.get_shader_resources();

	int32_t binding_offset = 0;

	if (isVulkanMode_)
	{
		binding_offset += 1;
	}

	if (shaderModel_ <= 330 || isVulkanMode_)
	{
		for (auto& remap : compiler.get_combined_image_samplers())
		{
			compiler.set_name(remap.combined_id, spirv_cross::join("Sampler_", compiler.get_name(remap.sampler_id)));
			auto location = compiler.get_decoration(remap.sampler_id, spv::DecorationBinding);
			compiler.set_decoration(remap.combined_id, spv::DecorationLocation, location);
		}
	}

	for (auto& resource : resources.sampled_images)
	{
		auto i = compiler.get_decoration(resource.id, spv::DecorationLocation);
		compiler.set_decoration(resource.id, spv::DecorationBinding, binding_offset + i);

		if (spirv->GetStage() == ShaderStageType::Vertex)
		{
			if (isVulkanMode_)
			{
				compiler.set_decoration(resource.id, spv::DecorationDescriptorSet, 0);
			}
		}
		else if (spirv->GetStage() == ShaderStageType::Pixel)
		{
			if (isVulkanMode_)
			{
				compiler.set_decoration(resource.id, spv::DecorationDescriptorSet, 1);
			}
		}
	}

	int cb_ind = 0;

	for (auto& resource : resources.uniform_buffers)
	{
		if (!isVulkanMode_)
		{
			if (spirv->GetStage() == ShaderStageType::Vertex)
			{
				compiler.set_name(resource.id, "CBVS" + std::to_string(cb_ind));
			}
			else if (spirv->GetStage() == ShaderStageType::Pixel)
			{
				compiler.set_name(resource.id, "CBPS" + std::to_string(cb_ind));
			}
		}

		if (spirv->GetStage() == ShaderStageType::Vertex)
		{
			if (isVulkanMode_)
			{
				compiler.set_decoration(resource.id, spv::DecorationBinding, 0);
				compiler.set_decoration(resource.id, spv::DecorationDescriptorSet, 0);
			}
		}
		else if (spirv->GetStage() == ShaderStageType::Pixel)
		{
			if (isVulkanMode_)
			{
				compiler.set_decoration(resource.id, spv::DecorationBinding, 0);
				compiler.set_decoration(resource.id, spv::DecorationDescriptorSet, 1);
			}
		}

		cb_ind++;
	}

	spirv_cross::CompilerGLSL::Options options;
	options.version = shaderModel_;
	if (shaderModel_ >= 420)
	{
		options.enable_420pack_extension = true;

		if (!isVulkanMode_)
		{
			options.separate_shader_objects = true;
		}
	}

	options.emit_uniform_buffer_as_plain_uniforms = plain_;
	options.vulkan_semantics = isVulkanMode_;
	options.es = isES_;

	compiler.set_common_options(options);

	code_ = compiler.compile();

	if (!isVulkanMode_ && shaderModel_ <= 330)
	{
		if (spirv->GetStage() == ShaderStageType::Vertex)
		{
			code_ = Replace(code_, "_entryPointOutput_", "_VSPS_");
		}
		else if (spirv->GetStage() == ShaderStageType::Pixel)
		{
			code_ = Replace(code_, "Input_", "_VSPS_");
		}
	}

	return true;
}

class ReflectionCompiler : public spirv_cross::Compiler
{
public:
	ReflectionCompiler(const std::vector<uint32_t>& data) : Compiler(data) {}
	virtual ~ReflectionCompiler() = default;

	size_t get_member_count(uint32_t id) const
	{
		const spirv_cross::Meta& m = ir.meta.at(id);
		return m.members.size();
	}

	spirv_cross::SPIRType get_member_type(const spirv_cross::SPIRType& struct_type, uint32_t index) const
	{
		return get<spirv_cross::SPIRType>(struct_type.member_types[index]);
	}
};

bool SPIRVReflection::Transpile(const std::shared_ptr<SPIRV>& spirv)
{
	Textures.clear();
	Uniforms.clear();

	ReflectionCompiler compiler(spirv->GetData());
	spirv_cross::ShaderResources resources = compiler.get_shader_resources();

	// Texture
	for (const auto& sampler : resources.separate_images)
	{
		ShaderReflectionTexture t;
		t.Name = sampler.name;
		t.Offset = compiler.get_decoration(sampler.id, spv::DecorationBinding);
		Textures.push_back(t);
	}

	// Uniform
	for (const auto& resource : resources.uniform_buffers)
	{
		auto count = compiler.get_member_count(resource.base_type_id);
		auto spirvType = compiler.get_type(resource.type_id);

		for (size_t i = 0; i < count; i++)
		{
			ShaderReflectionUniform u;
			auto memberType = compiler.get_member_type(spirvType, static_cast<uint32_t>(i));
			u.Name = compiler.get_member_name(resource.base_type_id, static_cast<uint32_t>(i));
			u.Size = static_cast<int32_t>(compiler.get_declared_struct_member_size(spirvType, static_cast<uint32_t>(i)));
			u.Offset = compiler.get_member_decoration(resource.base_type_id, static_cast<uint32_t>(i), spv::DecorationOffset);
			Uniforms.push_back(u);
		}
	}

	return true;
}

SPIRVGenerator::SPIRVGenerator(const std::function<std::vector<std::uint8_t>(std::string)>& onLoad) : onLoad_(onLoad)
{
	glslang::InitializeProcess();
}

SPIRVGenerator::~SPIRVGenerator() { glslang::FinalizeProcess(); }

std::shared_ptr<SPIRV> SPIRVGenerator::Generate(
	const char* path, const char* code, std::vector<SPIRVGeneratorMacro> macros, ShaderStageType shaderStageType, bool isYInverted)
{
	std::string codeStr(code);
	glslang::TProgram program;
	TBuiltInResource resources = glslang::DefaultTBuiltInResource;
	auto shaderStage = GetGlslangShaderStage(shaderStageType);

	glslang::TShader shader = glslang::TShader(shaderStage);
	shader.setEnvInput(glslang::EShSourceHlsl, shaderStage, glslang::EShClientOpenGL, glslang::EShTargetOpenGL_450);
	shader.setEnvClient(glslang::EShClientOpenGL, glslang::EShTargetOpenGL_450);
	shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

	if (isYInverted)
	{
		shader.setInvertY(true);
	}

	const char* shaderStrings[1];
	shaderStrings[0] = codeStr.c_str();
	shader.setEntryPoint("main");

	std::string macro;

	for (auto& m : macros)
	{
		macro += "#define " + m.Name + " " + m.Content + "\n";
	}

	shader.setPreamble(macro.c_str());
	// shader->setAutoMapBindings(true);
	// shader->setAutoMapLocations(true);

	shader.setStrings(shaderStrings, 1);
	const auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgReadHlsl | EShOptFull);

	DirStackFileIncluder includer(onLoad_);
	includer.pushExternalLocalDirectory(dirnameOf(path));

	int defaultVersion = 110;
	if (!shader.parse(&resources, defaultVersion, false, messages, includer))
	{
		return std::make_shared<SPIRV>(shader.getInfoLog());
	}

	program.addShader(&shader);

	if (!program.link(messages))
	{
		return std::make_shared<SPIRV>(program.getInfoLog());
	}

	std::vector<unsigned int> spirv;
	glslang::SpvOptions spvOptions;
	spvOptions.optimizeSize = true;
	spvOptions.disableOptimizer = false;

	glslang::GlslangToSpv(*program.getIntermediate(shaderStage), spirv, &spvOptions);

	return std::make_shared<SPIRV>(spirv, shaderStageType);
}

} // namespace LLGI
