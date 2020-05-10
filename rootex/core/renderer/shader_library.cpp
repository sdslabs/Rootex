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

	Shader* newShader = new Shader(vertexPath, pixelPath, vertexBufferFormat);
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
		BufferFormat defaultBufferFormat;
		defaultBufferFormat.push(VertexBufferElement::Type::POSITION, "POSITION");
		MakeShader(ShaderType::Color, L"rootex/assets/shaders/vertex_shader.cso", L"rootex/assets/shaders/pixel_shader.cso", defaultBufferFormat);
	}
	{
		BufferFormat diffuseTextureBufferFormat;
		diffuseTextureBufferFormat.push(VertexBufferElement::Type::POSITION, "POSITION");
		diffuseTextureBufferFormat.push(VertexBufferElement::Type::POSITION, "NORMAL");
		diffuseTextureBufferFormat.push(VertexBufferElement::Type::TEXCOORD, "TEXCOORD");
		MakeShader(ShaderType::Texture, L"rootex/assets/shaders/texture_vertex_shader.cso", L"rootex/assets/shaders/texture_pixel_shader.cso", diffuseTextureBufferFormat);
	}
}

void ShaderLibrary::DestroyShaders()
{
	s_Shaders.clear();
}

Shader* ShaderLibrary::GetColorShader()
{
	return s_Shaders[ShaderType::Color].get();
}

TextureShader* ShaderLibrary::GetTextureShader()
{
	return reinterpret_cast<TextureShader*>(s_Shaders[ShaderType::Texture].get());
}
