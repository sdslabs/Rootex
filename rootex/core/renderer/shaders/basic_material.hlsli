#ifndef BASIC_MATERIAL_HLSLI
#define BASIC_MATERIAL_HLSLI

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
};

#endif
