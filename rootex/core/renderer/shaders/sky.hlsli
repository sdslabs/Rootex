#ifndef SKY_HLSLI
#define SKY_HLSLI

#include "basic_material.hlsli"

float3 GetReflectionFromSky(float4 finalColor, float3 toEye, float3 normal, TextureCube sky, SamplerState sampleType, float reflectivity, float affectedBySky, float fresnelPower, float fresnelBrightness)
{
	float3 incident = -toEye;
	float3 reflectionVector = reflect(incident, normal);
	float fresnelEffect = pow(saturate(1 + dot(incident, normal)), fresnelPower) * fresnelBrightness;
	float4 reflectionColor = sky.Sample(sampleType, reflectionVector) * fresnelEffect;
	return lerp(finalColor, reflectionColor, reflectivity * affectedBySky).rgb;
}

float3 GetRefractionFromSky(float4 finalColor, float3 normal, float4 worldPosition, float3 cameraPos, TextureCube sky, SamplerState sampleType, float refractionConstant, float refractivity, float affectedBySky)
{
	float3 refractionIncident = normalize(worldPosition.xyz - cameraPos);
	float3 refractionReflect = refract(refractionIncident, normalize(normal), refractionConstant);
	float4 refractionColor = sky.Sample(sampleType, refractionReflect);
	return lerp(finalColor, refractionColor, refractivity * affectedBySky).rgb;
}

#endif
