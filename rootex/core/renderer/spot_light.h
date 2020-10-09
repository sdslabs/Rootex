#pragma once

#include "common/common.h"

struct SpotLight
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
	/// Increasing spot increases the angular attenuation wrt axis
	float spot;
	/// Lighting effect clipped for angle > angleRange
	float angleRange;
};
