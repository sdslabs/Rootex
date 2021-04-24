#pragma once

#include "core/renderer/shader.h"
#include "resource_file.h"
#include "image_resource_file.h"
#include "renderer/shaders/register_locations_pixel_shader.h"
#include "renderer/shaders/register_locations_vertex_shader.h"

class MaterialResourceFile : public ResourceFile
{
	bool m_IsAlpha = false;

protected:
	MaterialResourceFile(const Type& type, const FilePath& path);

public:
	bool saveMaterialData(const JSON::json& j);

	virtual const Shader* getShader() const = 0;
	virtual void bindShader() = 0;
	virtual void bindTextures() = 0;
	virtual void bindSamplers() = 0;
	virtual void bindVSCB() = 0;
	virtual void bindPSCB() = 0;

	virtual JSON::json getJSON() const;
	virtual ID3D11ShaderResourceView* getPreview() const = 0;

	void readJSON(const JSON::json& j);

	void setAlpha(bool enabled) { m_IsAlpha = enabled; }
	bool isAlpha() const { return m_IsAlpha; }

	void draw() override;
};

/// Kept separate from the main data buffer class because it needs proper packing
struct PerModelPSCBData
{
	Color color;
	int isLit = 0;
	/// Describes brightness of specular spot, high for metallic material
	float specularIntensity = 2.0f;
	/// Describes angular fall-off of specular spot, high for metallic material
	float specularPower = 30.0f;
	float reflectivity = 0.0f;
	float refractionConstant = 0.5f;
	float refractivity = 0.0f;
	int affectedBySky = 0;
	int hasNormalMap = 0;
};

struct PerFrameCustomPSCBData
{
	float timeMs;
	float deltaTimeMs;
	Vector2 resolution;
	Vector2 mouse;
	float pad[2];
};

struct PerModelVSCBData
{
	Matrix model;
	Matrix modelInverseTranspose;
	PerModelVSCBData() = default;
	PerModelVSCBData(const Matrix& modelMatrix)
	{
		model = modelMatrix.Transpose();
		modelInverseTranspose = modelMatrix.Invert();
	}
};

struct PerModelAnimationVSCBData
{
	Matrix m_BoneTransforms[MAX_BONES];
	PerModelAnimationVSCBData() = default;
	PerModelAnimationVSCBData(const Vector<Matrix>& transforms)
	{
		int N = std::min(MAX_BONES, (int)transforms.size());
		for (int i = 0; i < N; i++)
		{
			m_BoneTransforms[i] = transforms[i].Transpose();
		}
	}
};

struct BasicMaterialData
{
	String diffuseImage;
	String normalImage;
	String specularImage;
	String lightmapImage;
	PerModelPSCBData pixelBufferData;
};

void to_json(JSON::json& j, const BasicMaterialData& b);
void from_json(const JSON::json& j, BasicMaterialData& b);

struct SkyMaterialData
{
	String skyImage;
};

void to_json(JSON::json& j, const SkyMaterialData& s);
void from_json(const JSON::json& j, SkyMaterialData& s);

struct CustomMaterialData
{
	String pixelShaderPath;
	Vector<Ref<ImageResourceFile>> textures;
};

void to_json(JSON::json& j, const CustomMaterialData& s);
void from_json(const JSON::json& j, CustomMaterialData& s);
