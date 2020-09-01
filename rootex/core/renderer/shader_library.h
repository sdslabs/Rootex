#pragma once

#include "common/common.h"
#include "shader.h"

/// Does Shader caching
class ShaderLibrary
{
	enum class ShaderType
	{
		Basic,
		Sky
	};

private:
	/// New shaders are stored in this hash map, existing once are retrieved
	static HashMap<ShaderType, Ptr<Shader>> s_Shaders;

	/// Deals with the hash map
	static Shader* MakeShader(ShaderType shaderType, const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);

public:
	/// Load all shaders
	static void MakeShaders();
	/// Unload all shaders
	static void DestroyShaders();

	static BasicShader* GetBasicShader();
	static SkyShader* GetSkyShader();
};
