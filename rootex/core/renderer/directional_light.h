#pragma once

#include "common/common.h"

struct DirectionalLight
{
	/// Diffuse intensity of light
	float diffuseIntensity;
	/// Diffuse color of light
	Color diffuseColor;
	/// Ambient color of light
	Color ambientColor;
};
