#include "register_locations_pixel_shader.h"
#include "light.hlsli"
#include "basic_material.hlsli"
#include "sky.hlsli"

#define NUMBEROFSHADES float(2.)

TextureCube SkyTexture : register(SKY_PS_HLSL);
Texture2D ShaderTexture : register(DIFFUSE_PS_HLSL);
Texture2D NormalTexture : register(NORMAL_PS_HLSL);
Texture2D SpecularTexture : register(SPECULAR_PS_HLSL);
Texture2D LightmapTexture : register(LIGHTMAP_PS_HLSL);
//Texture2D ShaderTexture : register(CUSTOM_TEXTURE_0_PS_HLSL);
SamplerState SampleType : register(SAMPLER_PS_HLSL);

struct PixelInputType
{
	float4 screenPosition : SV_POSITION;
	float3 normal : NORMAL;
	float4 worldPosition : POSITION;
	float2 tex : TEXCOORD0;
	float fogFactor : FOG;
	float3 tangent : TANGENT;
	float4 color : COLOR;
};

cbuffer CBuf : register(PER_OBJECT_PS_HLSL)
{
	BasicMaterial material;
};

cbuffer CBuf : register(PER_MODEL_PS_HLSL)
{
	int staticPointLightAffectingCount = 0;
	int staticPointsLightsAffecting[MAX_STATIC_POINT_LIGHTS_AFFECTING_1_OBJECT];
};

float quantize(float intensity)
{
	return floor(intensity * NUMBEROFSHADES) / NUMBEROFSHADES;
}

float4 GetCelColorFromPointLight(PointLightInfo pointLight, float3 toEye, float3 normal, float4 worldPosition, float4 materialColor, float3 specularColor, float specPow, float specularIntensity, float isLit)
{
	float dist = distance(pointLight.lightPos, worldPosition.xyz);

	float4 totalColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	float3 relative = pointLight.lightPos - worldPosition.xyz;
	float3 normalizedRelative = relative / dist;
	float att = 1.0f / (pointLight.attConst + pointLight.attLin * dist + pointLight.attQuad * (dist * dist));
	float cosAngle = max(0.0f, dot(normalizedRelative, normal));

	float celIntensity = quantize(pointLight.diffuseIntensity);
	float3 diffuse = (pointLight.diffuseColor * celIntensity * quantize(cosAngle)).rgb;
	float3 reflected = floor(reflect(-normalizedRelative, normal) * NUMBEROFSHADES) / NUMBEROFSHADES;
	float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
	float3 specular = specularColor * specFactor * specularIntensity;

	totalColor = float4(((diffuse + (float3)pointLight.ambientColor) * (float3)materialColor) * att, 0.0f);
	return lerp(float4(0.0f, 0.0f, 0.0f, 0.0f), totalColor * isLit, dist < pointLight.range);
}

float4 GetCelColorFromDirectionalLight(DirectionalLightInfo directionalLight, float3 toEye, float3 normal, float4 materialColor, float3 specularColor, float specPow, float specularIntensity, float isLit)
{
	float3 direction = normalize(directionalLight.direction);
	float cosAngle = max(0.0f, dot(-direction, normal));

	float celIntensity = quantize(directionalLight.diffuseIntensity);
	float3 diffuse = (directionalLight.diffuseColor * celIntensity * quantize(cosAngle)).rgb;
	float3 reflected = reflect(-direction, normal);
	float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
	float3 specular = specularColor * specFactor * specularIntensity;
	float4 totalColor = float4((diffuse + (float3)directionalLightInfo.ambientColor) * (float3)materialColor, 0.0f);
	return totalColor * isLit;
}

float4 GetCelColorFromSpotLight(SpotLightInfo spotLight, float3 toEye, float3 normal, float4 worldPosition, float4 materialColor, float3 specularColor, float specPow, float specularIntensity, float isLit)
{
	float dist = distance(spotLight.lightPos, worldPosition.xyz);
	float isInRange = ceil(saturate(dist / spotLight.range));

	float4 totalColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	float3 relative = spotLight.lightPos - worldPosition.xyz;
	float3 normalizedRelative = relative / dist;
	float cosAngle = max(0.0f, dot(normalizedRelative, normal));
	float rangeAngle = max(dot(-normalizedRelative, spotLight.direction), 0.0f);
	float att = 1.0f / (spotLight.attConst + spotLight.attLin * dist + spotLight.attQuad * (dist * dist));

    float celIntensity = quantize(spotLight.diffuseIntensity);
	float3 diffuse = (spotLight.diffuseColor * spotLight.diffuseIntensity * quantize(cosAngle)).rgb;
	float3 reflected = reflect(-normalizedRelative, normal);
	float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
	float3 specular = specularColor * specFactor * specularIntensity;
	float spotFactor = pow(rangeAngle, spotLight.spot);

	totalColor = float4(((diffuse + (float3)spotLight.ambientColor) * (float3)materialColor) * att * spotFactor, 0.0f);
	return lerp(materialColor, totalColor * isLit, isInRange);
}
float4 main(PixelInputType input)
    : SV_TARGET
{
	float4 materialColor = ShaderTexture.Sample(SampleType, input.tex) * material.color * input.color;
	float4 finalColor = materialColor;

	clip(finalColor.a - 0.0001f);

	float3 toEye = normalize(cameraPos - (float3)input.worldPosition);

	finalColor.rgb = lerp(finalColor.rgb, float3(0.0f, 0.0f, 0.0f), 1);
	input.normal = normalize(input.normal);

	if (material.hasNormalMap)
	{
		float3 normalMapSample = NormalTexture.Sample(SampleType, input.tex).rgb;
		float3 uncompressedNormal = 2.0f * normalMapSample - 1.0f;
		float3 N = input.normal;
		float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
		float3 B = cross(N, T);

		float3x3 TBN = float3x3(T, B, N);

		input.normal = mul(uncompressedNormal, TBN);
	}

	float3 specularColor = SpecularTexture.Sample(SampleType, input.tex).rgb;
	for (int i = 0; i < pointLightCount; i++)
	{
		finalColor += saturate(GetCelColorFromPointLight(pointLightInfos[i], toEye, input.normal, input.worldPosition, materialColor, specularColor, material.specPow, material.specularIntensity, 1));
	}

	for (i = 0; i < staticPointLightAffectingCount; i++)
	{
		finalColor += saturate(GetCelColorFromPointLight(staticPointLightInfos[staticPointsLightsAffecting[i]], toEye, input.normal, input.worldPosition, materialColor, specularColor, material.specPow, material.specularIntensity, 1));
	}

	finalColor += saturate(GetCelColorFromDirectionalLight(directionalLightInfo, toEye, input.normal, materialColor, specularColor, material.specPow, material.specularIntensity, 1));

	for (i = 0; i < spotLightCount; i++)
	{
		finalColor += saturate(GetCelColorFromSpotLight(spotLightInfos[i], toEye, input.normal, input.worldPosition, materialColor, specularColor, material.specPow, material.specularIntensity, 1));
	}

	finalColor.rgb = GetReflectionFromSky(finalColor, toEye, input.normal, SkyTexture, SampleType, material.reflectivity, material.affectedBySky, material.fresnelPower, material.fresnelBrightness);
	finalColor.rgb = GetRefractionFromSky(finalColor, input.normal, input.worldPosition, cameraPos, SkyTexture, SampleType, material.refractionConstant, material.refractivity, material.affectedBySky);

	finalColor = finalColor * LightmapTexture.Sample(SampleType, input.tex);
	finalColor.rgb = (lerp(fogColor, finalColor, input.fogFactor)).rgb;

	return finalColor;
	float intensity = (finalColor.r + finalColor.g + finalColor.b) / 3.; //Maybe use dot(dirOfLight, surfaceNormal)
	const float SHADES = 3.0f;
	float shade = floor(intensity * float(SHADES));
	float brightnessOfShade = shade / float(SHADES);
	float factor = intensity / brightnessOfShade;
	finalColor.rgb /= factor;
	return finalColor;
}
