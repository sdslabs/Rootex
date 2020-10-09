#pragma once

#include "common/common.h"

struct PointLight
{
	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r)
	float attConst;
	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r)
	float attLin;
	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r)
	float attQuad;
	/// Lighting effect clipped for distance > range
	float range;
	/// Diffuse intensity of light
	float diffuseIntensity;
	/// Diffuse color of light
	Color diffuseColor;
	/// Ambient color of light
	Color ambientColor;
};
