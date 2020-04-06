#pragma once

#include "common/common.h"
#include "shader.h"

class ShaderLibrary
{
	enum class ShaderType
	{
		Default,
		Diffuse,
		CPUParticles
	};

private:
	static HashMap<ShaderType, Ptr<Shader>> s_Shaders;

	static Shader* MakeShader(ShaderType shaderType, const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);

public:
	static void MakeShaders();
	static void DestroyShaders();

	static Shader* GetDefaultShader();
	static DiffuseShader* GetDiffuseShader();
	static CPUParticlesShader* GetCPUParticlesShader();
};
