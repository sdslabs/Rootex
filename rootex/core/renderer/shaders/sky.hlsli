#ifndef SKY_HLSLI
#define SKY_HLSLI

#include "basic_material.hlsli"

float3 GetReflectionFromSky(float4 finalColor, float3 toEye, float3 normal, TextureCube sky, SamplerState sampleType, BasicMaterial material)
{
    float3 incident = -toEye;
    float3 reflectionVector = reflect(incident, normal);
    float4 reflectionColor = sky.Sample(sampleType, reflectionVector);
    return lerp(finalColor, reflectionColor, material.reflectivity * material.affectedBySky).rgb;
}

float3 GetRefractionFromSky(float4 finalColor, float3 normal, float4 worldPosition, float3 cameraPos, TextureCube sky, SamplerState sampleType, BasicMaterial material)
{
    float3 refractionIncident = normalize(worldPosition.xyz - cameraPos);
    float3 refractionReflect = refract(refractionIncident, normalize(normal), material.refractionConstant);
    float4 refractionColor = sky.Sample(sampleType, refractionReflect);
    return lerp(finalColor, refractionColor, material.refractivity * material.affectedBySky).rgb;
}

#endif
