#include "shader_library.h"

HashMap<ShaderLibrary::ShaderType, Ptr<Shader>> ShaderLibrary::s_Shaders;

Shader* ShaderLibrary::MakeShader(ShaderType shaderType, const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
{
	auto& findIt = s_Shaders.find(shaderType);
	if (findIt != s_Shaders.end())
	{
		WARN("Duplicate shader assigned. Older one returned: " + std::to_string((int)shaderType));
		return s_Shaders[shaderType].get();
	}

	Shader* newShader = nullptr;
	switch (shaderType)
	{
	case ShaderLibrary::ShaderType::Basic:
		newShader = new BasicShader(vertexPath, pixelPath, vertexBufferFormat);
		break;
	case ShaderLibrary::ShaderType::Sky:
		newShader = new SkyShader(vertexPath, pixelPath, vertexBufferFormat);
		break;
	default:
		WARN("Unknown shader type found");
		break;
	}

	s_Shaders[shaderType].reset(newShader);

	return newShader;
}

void ShaderLibrary::MakeShaders()
{
	if (s_Shaders.size() > 1)
	{
		WARN("Tried constructing already constructed shader objects. Operation ignored");
		return;
	}
	{
		BufferFormat basicBufferFormat;
		basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION");
		basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "NORMAL");
		basicBufferFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD");
		basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "TANGENT");
		basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "BITANGENT");
		MakeShader(ShaderType::Basic, L"rootex/assets/shaders/basic_vertex_shader.cso", L"rootex/assets/shaders/basic_pixel_shader.cso", basicBufferFormat);
	}
	{
		BufferFormat skyFormat;
		skyFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION");
		MakeShader(ShaderType::Sky, L"rootex/assets/shaders/sky_vertex_shader.cso", L"rootex/assets/shaders/sky_pixel_shader.cso", skyFormat);
	}
}

void ShaderLibrary::DestroyShaders()
{
	s_Shaders.clear();
}

BasicShader* ShaderLibrary::GetBasicShader()
{
	return reinterpret_cast<BasicShader*>(s_Shaders[ShaderType::Basic].get());
}

SkyShader* ShaderLibrary::GetSkyShader()
{
	return reinterpret_cast<SkyShader*>(s_Shaders[ShaderType::Sky].get());
}
