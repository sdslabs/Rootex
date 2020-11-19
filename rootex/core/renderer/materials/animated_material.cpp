#include "animated_material.h"

#include "renderer/shaders/register_locations_vertex_shader.h"
#include "resource_loader.h"
#include "renderer/shader_library.h"
#include "framework/systems/render_system.h"

AnimatedMaterial::AnimatedMaterial(bool isAlpha, const String& imagePath, const String& normalImagePath, const String& specularImagePath, bool isNormal, Color color, bool isLit, float specularIntensity, float specularPower, float reflectivity, float refractionConstant, float refractivity, bool affectedBySky)
    : BasicMaterial(isAlpha, imagePath, normalImagePath, specularImagePath, isNormal, color, isLit, specularIntensity, specularPower, reflectivity, refractionConstant, refractivity, affectedBySky)
    , m_AnimationShader(ShaderLibrary::GetAnimationShader())
{
	m_Shader = ShaderLibrary::GetAnimationShader();
	m_TypeName = AnimatedMaterial::s_MaterialName;
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

Material* AnimatedMaterial::CreateDefault()
{
	return new AnimatedMaterial(false, "rootex/assets/white.png", "", "rootex/assets/white.png", false, Color(0.5f, 0.5f, 0.5f, 1.0f), true, 0.5f, 30.0f, 0.1f, 0.8f, 0.1f, true);
}

Material* AnimatedMaterial::Create(const JSON::json& materialData)
{
	bool isLit = materialData["isLit"];
	float specularIntensity = 2.0f;
	float specularPower = 30.0f;
	if (isLit)
	{
		specularIntensity = (float)materialData["specularIntensity"];
		specularPower = (float)materialData["specularPower"];
	}
	AnimatedMaterial* material = dynamic_cast<AnimatedMaterial*>(CreateDefault());
	material->m_IsLit = materialData["isLit"];
	if (material->m_IsLit)
	{
		material->m_SpecularIntensity = (float)materialData["specularIntensity"];
		material->m_SpecularPower = (float)materialData["specularPower"];
	}
	float reflectivity = 0.5f;
	if (materialData.find("reflectivity") != materialData.end())
	{
		reflectivity = materialData["reflectivity"];
	}
	float refractionConstant = 0.0f;
	if (materialData.find("refractionConstant") != materialData.end())
	{
		refractionConstant = materialData["refractionConstant"];
	}
	float refractivity = 0.0f;
	if (materialData.find("refractivity") != materialData.end())
	{
		refractivity = materialData["refractivity"];
	}
	bool affectedBySky = false;
	if (materialData.find("affectedBySky") != materialData.end())
	{
		affectedBySky = materialData["affectedBySky"];
	}
	bool isAlpha = false;
	if (materialData.find("isAlpha") != materialData.end())
	{
		isAlpha = materialData["isAlpha"];
	}
	bool isNormal = false;
	String normalImageFile = "";
	if (materialData.find("isNormal") != materialData.end())
	{
		isNormal = materialData["isNormal"];
		if (isNormal)
		{
			normalImageFile = materialData["normalImageFile"];
		}
	}
	String specularImageFile = "rootex/assets/white.png";
	if (materialData.find("specularImageFile") != materialData.end())
	{
		specularImageFile = materialData["specularImageFile"];
	}
	return new AnimatedMaterial(isAlpha, (String)materialData["imageFile"], normalImageFile, specularImageFile, isNormal, Color((float)materialData["color"]["r"], (float)materialData["color"]["g"], (float)materialData["color"]["b"], (float)materialData["color"]["a"]), isLit, specularIntensity, specularPower, reflectivity, refractionConstant, refractivity, affectedBySky);
}

void AnimatedMaterial::setVSConstantBuffer(const VSAnimationConstantBuffer& constantBuffer)
{
	Material::SetVSConstantBuffer<VSAnimationConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Animation], BONES_VS_CPP);
}

void AnimatedMaterial::bind()
{
	m_AnimationShader->set(m_DiffuseImageFile->getTexture().get(), DIFFUSE_PS_CPP);
	if (m_IsNormal)
	{
		m_AnimationShader->set(m_NormalImageFile->getTexture().get(), NORMAL_PS_CPP); 
	}
	m_AnimationShader->set(m_SpecularImageFile->getTexture().get(), SPECULAR_PS_CPP);
	Matrix currentModelMatrix = RenderSystem::GetSingleton()->getCurrentMatrix();
	BasicMaterial::setVSConstantBuffer(VSDiffuseConstantBuffer(currentModelMatrix));

	PSDiffuseConstantBufferMaterial objectPSCB;
	objectPSCB.affectedBySky = m_IsAffectedBySky;
	objectPSCB.color = m_Color;
	objectPSCB.hasNormalMap = m_IsNormal;
	objectPSCB.isLit = m_IsLit;
	objectPSCB.reflectivity = m_Reflectivity;
	objectPSCB.refractionConstant = m_RefractionConstant;
	objectPSCB.refractivity = m_Refractivity;
	objectPSCB.specularIntensity = m_SpecularIntensity;
	objectPSCB.specularPower = m_SpecularPower;

	BasicMaterial::setPSConstantBuffer(objectPSCB);
}
