#pragma once

#include "renderer/material.h"

class CPUParticlesMaterial : public Material
{
public:
	enum class VertexConstantBufferType
	{
		Model,
		End
	};
	enum class PixelConstantBufferType
	{
		Color,
		End
	};
	CPUParticlesMaterial();
	~CPUParticlesMaterial() = default;

	void setPSConstantBuffer(const PSSolidConstantBuffer& constantBuffer);
	void setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer);

	void bind() override;
};
