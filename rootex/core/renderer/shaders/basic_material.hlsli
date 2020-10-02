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
	int staticPointsLightsAffecting[MAX_STATIC_POINT_LIGHTS_AFFECTING_1_OBJECT];
};

#endif
