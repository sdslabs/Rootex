#ifndef BASIC_MATERIAL_HLSLI
#define BASIC_MATERIAL_HLSLI

#include "register_locations_pixel_shader.h"

struct BasicMaterial
{
    float4 color;
    int isLit;
    float specularIntensity;
    float specPow;
    float reflectivity;
    float refractionConstant;
    float refractivity;
    int affectedBySky;
    int hasNormalMap;
	float fresnelPower;
	float fresnelBrightness;
};

#endif
