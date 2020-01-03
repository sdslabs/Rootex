#include "shader_library.h"

HashMap<String, Ptr<Shader>> ShaderLibrary::s_Shaders;

Shader* ShaderLibrary::MakeShader(const String& name, const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
{
	auto& findIt = s_Shaders.find(name); 
	if (findIt != s_Shaders.end())
	{
		WARN("Duplicate m_Shader assigned. Older one returned: " + name);
		return s_Shaders[name].get();
	}

	Shader* newShader = new Shader(vertexPath, pixelPath, vertexBufferFormat);
	s_Shaders[name].reset(newShader);

	return newShader;
}

Shader* ShaderLibrary::GetShader(const String& name)
{
	auto& findIt = s_Shaders.find(name);
	if (findIt == s_Shaders.end())
	{
		return nullptr;
	}

	return findIt->second.get();
}
