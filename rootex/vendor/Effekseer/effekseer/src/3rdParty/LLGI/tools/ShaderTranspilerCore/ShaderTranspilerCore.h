
#pragma once

#include <stdint.h>

#include "../../src/LLGI.Base.h"
#include <memory>
#include <string>
#include <vector>

namespace LLGI
{

struct ShaderReflectionUniform
{
	std::string Name;
	int32_t Offset = 0;
	int32_t Size = 0;
};

struct ShaderReflectionTexture
{
	std::string Name;
	int32_t Offset = 0;
};

class SPIRV
{
private:
	std::vector<uint32_t> data_;
	std::string error_;
	ShaderStageType shaderStage_;

public:
	SPIRV(const std::vector<uint32_t>& data, ShaderStageType shaderStage);

	SPIRV(const std::string& error);

	ShaderStageType GetStage() const;

	const std::vector<uint32_t>& GetData() const;

	std::string GetError() const { return error_; }
};

class SPIRVTranspiler
{
protected:
	std::string code_;
	std::string errorCode_;

public:
	SPIRVTranspiler() = default;
	virtual ~SPIRVTranspiler() = default;

	virtual bool Transpile(const std::shared_ptr<SPIRV>& spirv);
	std::string GetErrorCode() const;
	std::string GetCode() const;
};

/**
	@brief it doesn't work currently
*/

class SPIRVToHLSLTranspiler : public SPIRVTranspiler
{
	int32_t shaderModel_ = 30;
	bool isDX12_ = false;

public:
	SPIRVToHLSLTranspiler(int32_t shaderModel = 40, bool isDX12 = false);
	bool Transpile(const std::shared_ptr<SPIRV>& spirv) override;
};

class SPIRVToMSLTranspiler : public SPIRVTranspiler
{
public:
	bool Transpile(const std::shared_ptr<SPIRV>& spirv) override;
};

class SPIRVToGLSLTranspiler : public SPIRVTranspiler
{
private:
	bool isVulkanMode_ = false;
	bool isES_ = false;
	int32_t shaderModel_ = 420;
	bool plain_ = false;

public:
	SPIRVToGLSLTranspiler(bool isVulkanMode, int32_t shaderModel = 420, bool isES = false, bool plain = false)
		: isVulkanMode_(isVulkanMode), isES_(isES), shaderModel_(shaderModel), plain_(plain)
	{
	}

	bool Transpile(const std::shared_ptr<SPIRV>& spirv) override;
};

class SPIRVReflection : public SPIRVTranspiler
{
public:
	bool Transpile(const std::shared_ptr<SPIRV>& spirv) override;

	std::vector<ShaderReflectionUniform> Uniforms;

	std::vector<ShaderReflectionTexture> Textures;
};

class SPIRVGeneratorMacro
{
public:
	std::string Name;
	std::string Content;

	SPIRVGeneratorMacro() {}

	SPIRVGeneratorMacro(const char* name, const char* content) : Name(name), Content(content) {}
};

class SPIRVGenerator
{
private:
	std::function<std::vector<std::uint8_t>(std::string)> onLoad_;

public:
	SPIRVGenerator(const std::function<std::vector<std::uint8_t>(std::string)>& onLoad);

	~SPIRVGenerator();

	std::shared_ptr<SPIRV> Generate(
		const char* path, const char* code, std::vector<SPIRVGeneratorMacro> macros, ShaderStageType shaderStageType, bool isYInverted);
};

} // namespace LLGI
