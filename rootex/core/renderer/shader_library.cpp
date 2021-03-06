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
	case ShaderLibrary::ShaderType::Particles:
		newShader = new ParticlesShader(vertexPath, pixelPath, vertexBufferFormat);
		break;
	case ShaderLibrary::ShaderType::Sky:
		newShader = new SkyShader(vertexPath, pixelPath, vertexBufferFormat);
		break;
	case ShaderLibrary::ShaderType::Animation:
		newShader = new AnimationShader(vertexPath, pixelPath, vertexBufferFormat);
		break;
	case ShaderLibrary::ShaderType::FXAA:
		newShader = new FXAAShader(vertexPath, pixelPath, vertexBufferFormat);
		break;
	case ShaderLibrary::ShaderType::Luma:
		newShader = new LumaShader(vertexPath, pixelPath, vertexBufferFormat);
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
		basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "NORMAL", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		basicBufferFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "TANGENT", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		MakeShader(ShaderType::Basic, L"rootex/assets/shaders/basic_vertex_shader.cso", L"rootex/assets/shaders/basic_pixel_shader.cso", basicBufferFormat);
	}
	{
		BufferFormat particlesBufferFormat;
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "NORMAL", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "TANGENT", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_ROWX", D3D11_INPUT_PER_INSTANCE_DATA, 1, true, 1);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_ROWY", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_ROWZ", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_ROWW", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_INVERSE_TRANSPOSE_ROWX", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_INVERSE_TRANSPOSE_ROWY", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_INVERSE_TRANSPOSE_ROWZ", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_INVERSE_TRANSPOSE_ROWW", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
		particlesBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_COLOR", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
		MakeShader(ShaderType::Particles, L"rootex/assets/shaders/particles_vertex_shader.cso", L"rootex/assets/shaders/particles_pixel_shader.cso", particlesBufferFormat);
	}
	{
		BufferFormat skyFormat;
		skyFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		MakeShader(ShaderType::Sky, L"rootex/assets/shaders/sky_vertex_shader.cso", L"rootex/assets/shaders/sky_pixel_shader.cso", skyFormat);
	}
	{
		BufferFormat animationFormat;
		animationFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		animationFormat.push(VertexBufferElement::Type::FloatFloatFloat, "NORMAL", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		animationFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		animationFormat.push(VertexBufferElement::Type::FloatFloatFloat, "TANGENT", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		animationFormat.push(VertexBufferElement::Type::IntIntIntInt, "BONEINDICES", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		animationFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "BONEWEIGHTS", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		MakeShader(ShaderType::Animation, L"rootex/assets/shaders/animation_vertex_shader.cso", L"rootex/assets/shaders/basic_pixel_shader.cso", animationFormat);
	}
	{
		BufferFormat fxaaFormat;
		fxaaFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		fxaaFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		MakeShader(ShaderType::FXAA, L"rootex/assets/shaders/fxaa_vertex_shader.cso", L"rootex/assets/shaders/fxaa_pixel_shader.cso", fxaaFormat);
	}
	{
		BufferFormat lumaFormat;
		lumaFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		lumaFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
		MakeShader(ShaderType::Luma, L"rootex/assets/shaders/luma_vertex_shader.cso", L"rootex/assets/shaders/luma_pixel_shader.cso", lumaFormat);
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

ParticlesShader* ShaderLibrary::GetParticlesShader()
{
	return reinterpret_cast<ParticlesShader*>(s_Shaders[ShaderType::Particles].get());
}

SkyShader* ShaderLibrary::GetSkyShader()
{
	return reinterpret_cast<SkyShader*>(s_Shaders[ShaderType::Sky].get());
}

AnimationShader* ShaderLibrary::GetAnimationShader()
{
	return reinterpret_cast<AnimationShader*>(s_Shaders[ShaderType::Animation].get());
}

FXAAShader* ShaderLibrary::GetFXAAShader()
{
	return reinterpret_cast<FXAAShader*>(s_Shaders[ShaderType::FXAA].get());
}

LumaShader* ShaderLibrary::GetLumaShader()
{
	return reinterpret_cast<LumaShader*>(s_Shaders[ShaderType::Luma].get());
}
