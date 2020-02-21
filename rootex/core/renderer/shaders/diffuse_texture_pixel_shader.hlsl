Texture2D ShaderTexture;
SamplerState SampleType;

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float4 worldPosition : POSITION;
    float2 tex : TEXCOORD0;
};
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

cbuffer Lights : register(b0)
{
    int pointLightCount;
    PointLightInfo pointLightInfos[4];
    int directionLightPresent;
    DirectionalLightInfo directionalLightInfo;
    int spotLightCount;
    SpotLightInfo spotLightInfos[4];
};

cbuffer Material: register(b1)
{
    float specularIntensity;
    float specPow;
};

float4 main(PixelInputType input) : SV_TARGET
{    
    input.normal = normalize(input.normal);
    float4 materialColor = ShaderTexture.Sample(SampleType, input.tex);

    float4 finalColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    //TODO- FIX THIS HARDCODE
    float3 toEye = normalize(float3(0.0f, 0.0f, 4.0f) - (float3) input.worldPosition);
    
    for (int i = 0; i < pointLightCount; i++)
    {
        float3 relative = pointLightInfos[i].lightPos - (float3) input.worldPosition;
        float dist = length(relative);
        if(dist <= pointLightInfos[i].range)
        {
            float3 normalizedRelative = relative / dist;
            float att = 1.0f / (pointLightInfos[i].attConst + pointLightInfos[i].attLin * dist + pointLightInfos[i].attQuad * (dist * dist));
            float cosAngle = max(0.0f, dot(normalizedRelative, input.normal));
            float3 diffuse = pointLightInfos[i].diffuseColor * pointLightInfos[i].diffuseIntensity * cosAngle;
            float3 reflected = reflect(-normalizedRelative, input.normal);
            //TODO- FIX THIS
            float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
            float3 specular = specularIntensity * specFactor * diffuse;
        
            finalColor += float4(saturate((diffuse + (float3) pointLightInfos[i].ambientColor + specular) * (float3) materialColor * att), 0.0f);
        }
    }
    
    if (directionLightPresent == 1)
    {
        float3 direction = normalize(directionalLightInfo.direction);
        float cosAngle = max(0.0f, dot(-direction, input.normal));
        float3 diffuse = pointLightInfos[i].diffuseColor * directionalLightInfo.diffuseIntensity * cosAngle;
        float3 reflected = reflect(-direction, input.normal);
        float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
        float3 specular = specularIntensity * specFactor * diffuse;
        finalColor += float4(saturate((diffuse + (float3) directionalLightInfo.ambientColor + specular) * (float3) materialColor), 0.0f);
    }
    
    for (i = 0; i < spotLightCount; i++)
    {
        float3 relative = spotLightInfos[i].lightPos - (float3) input.worldPosition;
        float dist = length(relative);
        if (dist <= spotLightInfos[i].range)
        {
            float3 normalizedRelative = relative / dist;
            float cosAngle = max(0.0f, dot(normalizedRelative, input.normal));
            float rangeAngle = max(dot(-normalizedRelative, spotLightInfos[i].direction), 0.0f);
            if (rangeAngle > spotLightInfos[i].angleRange)
            {
                float att = 1.0f / (spotLightInfos[i].attConst + spotLightInfos[i].attLin * dist + spotLightInfos[i].attQuad * (dist * dist));
                float3 diffuse = spotLightInfos[i].diffuseColor * spotLightInfos[i].diffuseIntensity * cosAngle;
                float3 reflected = reflect(-normalizedRelative, input.normal);
                //TODO- FIX THIS
                float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
                float3 specular = specularIntensity * specFactor * diffuse;
            
                float spotFactor = pow(rangeAngle, spotLightInfos[i].spot);
        
                finalColor += float4(saturate((diffuse + (float3) spotLightInfos[i].ambientColor + specular) * (float3) materialColor * att * spotFactor), 0.0f);
            }
        }
    }
    
    return finalColor;
}
