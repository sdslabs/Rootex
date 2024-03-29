#ifndef LIGHT_HLSLI
#define LIGHT_HLSLI

#include "register_locations_pixel_shader.h"

struct PointLightInfo
{
    float4 ambientColor;
    float4 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
    float3 lightPos;
    float range;
};

struct DirectionalLightInfo
{
    float3 direction;
    float diffuseIntensity;
    float4 ambientColor;
    float4 diffuseColor;
};

struct SpotLightInfo
{
    float4 ambientColor;
    float4 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
    float3 lightPos;
    float range;
    float3 direction;
    float spot;
    float angleRange;
};

cbuffer StaticPointLights : register(PER_SCENE_PS_HLSL)
{
    PointLightInfo staticPointLightInfos[MAX_STATIC_POINT_LIGHTS];
}

cbuffer Lights : register(PER_FRAME_PS_HLSL)
{
    float3 cameraPos;
    int pointLightCount;
    PointLightInfo pointLightInfos[MAX_DYNAMIC_POINT_LIGHTS];
    int directionLightPresent;
    DirectionalLightInfo directionalLightInfo;
    int spotLightCount;
    SpotLightInfo spotLightInfos[MAX_DYNAMIC_SPOT_LIGHTS];
    float4 fogColor;
}

float4 GetColorFromPointLight(PointLightInfo pointLight, float3 lightPos, float3 toEye, float3 normal, float3 worldPosition, float4 materialColor, float3 specularColor, float specPow, float specularIntensity, float isLit)
{
    float dist = distance(lightPos, worldPosition.xyz);
    
    float4 totalColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    float3 relative = lightPos - worldPosition.xyz;
    float3 normalizedRelative = relative / dist;
    float att = 1.0f / (pointLight.attConst + pointLight.attLin * dist + pointLight.attQuad * (dist * dist));
    float cosAngle = max(0.0f, dot(normalizedRelative, normal));
    float3 diffuse = (pointLight.diffuseColor * pointLight.diffuseIntensity * cosAngle).rgb;
    float3 reflected = reflect(-normalizedRelative, normal);
    float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
    float3 specular = specularColor * specFactor * specularIntensity;

    totalColor = float4(((diffuse + (float3)pointLight.ambientColor) * (float3)materialColor + specular) * att, 0.0f);
    return lerp(float4(0.0f, 0.0f, 0.0f, 0.0f), totalColor * isLit, dist < pointLight.range);
}

float4 GetColorFromDirectionalLight(DirectionalLightInfo directionalLight, float3 lightDirection, float3 toEye, float3 normal, float4 materialColor, float3 specularColor, float specPow, float specularIntensity, float isLit)
{
    float3 direction = normalize(lightDirection);
    float cosAngle = max(0.0f, dot(-direction, normal));
    float3 diffuse = (directionalLight.diffuseColor * directionalLight.diffuseIntensity * cosAngle).rgb;
    float3 reflected = reflect(-direction, normal);
    float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
    float3 specular = specularColor * specFactor * specularIntensity;
    float4 totalColor = float4((diffuse + (float3)directionalLightInfo.ambientColor) * (float3)materialColor + specular, 0.0f);
    return totalColor * isLit;
}

float4 GetColorFromSpotLight(SpotLightInfo spotLight, float3 lightPos, float3 toEye, float3 normal, float3 worldPosition, float4 materialColor, float3 specularColor, float specPow, float specularIntensity, float isLit)
{
    float dist = distance(lightPos, worldPosition.xyz);
    float isInRange = ceil(saturate(dist / spotLight.range));
    
    float4 totalColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    float3 relative = lightPos - worldPosition.xyz;
    float3 normalizedRelative = relative / dist;
    float cosAngle = max(0.0f, dot(normalizedRelative, normal));
    float rangeAngle = max(dot(-normalizedRelative, spotLight.direction), 0.0f); // spotlight.direction Can be a potential bug
    float att = 1.0f / (spotLight.attConst + spotLight.attLin * dist + spotLight.attQuad * (dist * dist));
    float3 diffuse = (spotLight.diffuseColor * spotLight.diffuseIntensity * cosAngle).rgb;
    float3 reflected = reflect(-normalizedRelative, normal);
    float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
    float3 specular = specularColor * specFactor * specularIntensity;
    float spotFactor = pow(rangeAngle, spotLight.spot);

    totalColor = float4(((diffuse + (float3)spotLight.ambientColor) * (float3)materialColor + specular) * att * spotFactor, 0.0f);
    return lerp(materialColor, totalColor * isLit, isInRange);
}

#endif
