#ifndef LIGHT_HLSLI
#define LIGHT_HLSLI

#include "register_locations_pixel_shader.h"
#include "basic_material.hlsli"

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

cbuffer StaticPointLights : register(PER_LEVEL_PS_HLSL)
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

float4 GetColorFromPointLight(PointLightInfo pointLight, float3 toEye, float3 normal, float4 worldPosition, float4 materialColor, float3 specularColor, BasicMaterial material)
{
    float3 relative = pointLight.lightPos - (float3) worldPosition;
    float dist = length(relative);
    
    float4 totalColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (dist <= pointLight.range)
    {
        float3 normalizedRelative = relative / dist;
        float att = 1.0f / (pointLight.attConst + pointLight.attLin * dist + pointLight.attQuad * (dist * dist));
        float cosAngle = max(0.0f, dot(normalizedRelative, normal));
        float3 diffuse = (pointLight.diffuseColor * pointLight.diffuseIntensity * cosAngle).rgb;
        float3 reflected = reflect(-normalizedRelative, normal);
        float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), material.specPow);
        float3 specular = specularColor.r * specFactor * material.specularIntensity;
        
        totalColor = float4(((diffuse + (float3) pointLight.ambientColor) * (float3) materialColor + specular) * att, 0.0f);
    }
    return totalColor;
}

float4 GetColorFromDirectionalLight(DirectionalLightInfo directionalLight, float3 toEye, float3 normal, float4 materialColor, float3 specularColor, BasicMaterial material)
{
    float3 direction = normalize(directionalLight.direction);
    float cosAngle = max(0.0f, dot(-direction, normal));
    float3 diffuse = (directionalLight.diffuseColor * directionalLight.diffuseIntensity * cosAngle).rgb;
    float3 reflected = reflect(-direction, normal);
    float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), material.specPow);
    float3 specular = specularColor.r * specFactor * material.specularIntensity;
    float4 totalColor = float4((diffuse + (float3) directionalLightInfo.ambientColor) * (float3) materialColor + specular, 0.0f);
    return totalColor;
}

float4 GetColorFromSpotLight(SpotLightInfo spotLight, float3 toEye, float3 normal, float4 worldPosition, float4 materialColor, float3 specularColor, BasicMaterial material)
{
    float3 relative = spotLight.lightPos - (float3) worldPosition;
    float dist = length(relative);
    float4 totalColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (dist <= spotLight.range)
    {
        float3 normalizedRelative = relative / dist;
        float cosAngle = max(0.0f, dot(normalizedRelative, normal));
        float rangeAngle = max(dot(-normalizedRelative, spotLight.direction), 0.0f);
        if (rangeAngle > spotLight.angleRange)
        {
            float att = 1.0f / (spotLight.attConst + spotLight.attLin * dist + spotLight.attQuad * (dist * dist));
            float3 diffuse = (spotLight.diffuseColor * spotLight.diffuseIntensity * cosAngle).rgb;
            float3 reflected = reflect(-normalizedRelative, normal);
            float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), material.specPow);
            float3 specular = specularColor.r * specFactor * material.specularIntensity;
            float spotFactor = pow(rangeAngle, spotLight.spot);
        
            totalColor = float4(((diffuse + (float3) spotLight.ambientColor) * (float3) materialColor + specular) * att * spotFactor, 0.0f);
        }
    }
    return totalColor;
}

#endif
