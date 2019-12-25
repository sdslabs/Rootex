#pragma once

#include "common/common.h"
#include "shader.h"

class ShaderLibrary
{
	static HashMap<String, Ptr<Shader>> s_Shaders;

public:
	static Shader* MakeShader(const String& name, const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);
	static Shader* GetShader(const String& name);
};
