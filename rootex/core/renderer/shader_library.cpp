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
	if (!s_Shaders.empty())
	{
		WARN("Tried constructing already constructed shader objects. Operation ignored");
		return;
	}
	{
		BufferFormat defaultBufferFormat;
		defaultBufferFormat.push(VertexBufferElement::Type::POSITION, "POSITION");
		MakeShader(ShaderType::Default, L"rootex/assets/shaders/vertex_shader.cso", L"rootex/assets/shaders/pixel_shader.cso", defaultBufferFormat);
	}
	{
		BufferFormat diffuseTextureBufferFormat;
		diffuseTextureBufferFormat.push(VertexBufferElement::Type::POSITION, "POSITION");
		diffuseTextureBufferFormat.push(VertexBufferElement::Type::POSITION, "NORMAL");
		diffuseTextureBufferFormat.push(VertexBufferElement::Type::TEXCOORD, "TEXCOORD");
		MakeShader(ShaderType::Diffuse, L"rootex/assets/shaders/diffuse_texture_vertex_shader.cso", L"rootex/assets/shaders/diffuse_texture_pixel_shader.cso", diffuseTextureBufferFormat);
	}
	{
		BufferFormat cpuParticlesBufferFormat;
		cpuParticlesBufferFormat.push(VertexBufferElement::Type::POSITION, "POSITION");
		MakeShader(ShaderType::CPUParticles, L"rootex/assets/shaders/cpu_particles_vertex_shader.cso", L"rootex/assets/shaders/cpu_particles_pixel_shader.cso", cpuParticlesBufferFormat);
	}
	{
		BufferFormat gridBufferFormat;
		gridBufferFormat.push(VertexBufferElement::Type::POSITION, "POSITION");
		MakeShader(ShaderType::Grid, L"rootex/assets/shaders/grid_vertex_shader.cso", L"rootex/assets/shaders/grid_pixel_shader.cso", gridBufferFormat);
	}
}

void ShaderLibrary::DestroyShaders()
{
	s_Shaders.clear();
}

Shader* ShaderLibrary::GetDefaultShader()
{
	return s_Shaders[ShaderType::Default].get();
}

DiffuseShader* ShaderLibrary::GetDiffuseShader()
{
	return reinterpret_cast<DiffuseShader*>(s_Shaders[ShaderType::Diffuse].get());
}

CPUParticlesShader* ShaderLibrary::GetCPUParticlesShader()
{
	return reinterpret_cast<CPUParticlesShader*>(s_Shaders[ShaderType::CPUParticles].get());
}

GridShader* ShaderLibrary::GetGridShader()
{
	return reinterpret_cast<GridShader*>(s_Shaders[ShaderType::Grid].get());
}
