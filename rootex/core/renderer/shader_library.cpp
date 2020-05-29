#include "shader_library.h"

HashMap<ShaderLibrary::ShaderType, Ptr<Shader>> ShaderLibrary::s_Shaders;

Shader* ShaderLibrary::MakeShader(ShaderType shaderType, const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
{
	auto& findIt = s_Shaders.find(shaderType);
	if (findIt != s_Shaders.end())
	{
		WARN("Duplicate m_Shader assigned. Older one returned: " + std::to_string((int)shaderType));
		return s_Shaders[shaderType].get();
	}

	Shader* newShader = nullptr;
	switch (shaderType)
	{
	case ShaderLibrary::ShaderType::Basic:
		newShader = new BasicShader(vertexPath, pixelPath, vertexBufferFormat);
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
		MakeShader(ShaderType::Basic, L"rootex/assets/shaders/basic_vertex_shader.cso", L"rootex/assets/shaders/basic_pixel_shader.cso", basicBufferFormat);
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
