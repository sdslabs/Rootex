Texture2D ShaderTexture;
SamplerState SampleType;

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    matrix M : MATRIX;
    float4 worldPosition : POSITION;
    float2 tex : TEXCOORD0;
};
struct LightInfo
{
    float4 ambientColor;
    float4 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
    float3 lightPos;
};

cbuffer Lights : register(b0)
{
    int lightCount;
    LightInfo lightInfos[4];
};

cbuffer Material: register(b1)
{
    float4 materialColor;
    float specularIntensity;
    float specPow;
};

float4 main(PixelInputType input) : SV_TARGET
{    
    float4 finalColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    for (int i = 0; i < lightCount; i++)
    {
        float3 relative = (float3) input.worldPosition - lightInfos[i].lightPos;
        float dist = length(relative);
        float3 normalizedRelative = relative / dist;
        float att = 1.0f / (lightInfos[i].attConst + lightInfos[i].attLin * dist + lightInfos[i].attQuad * (dist * dist));
        float cosAngle = max(0.0f, dot(normalizedRelative, input.normal));
        float3 diffuse = lightInfos[i].diffuseColor * lightInfos[i].diffuseIntensity * cosAngle;
    
        float3 reflected = reflect(normalizedRelative, input.normal);
    
    //TODO- FIX THIS
        float3 toEye = float3(0.0f, 0.0f, 4.0f) - (float3) input.worldPosition;
    
        float specFactor = pow(max(dot(normalize(reflected), normalize(toEye)), 0.0f), specPow);
    
        float3 specular = specularIntensity * specFactor * diffuse;
        
        finalColor += float4(saturate((diffuse + (float3) lightInfos[i].ambientColor + specular) * (float3) materialColor * att), 0.0f);
        
    }
    
    return finalColor;
    
}
