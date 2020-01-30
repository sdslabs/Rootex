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

cbuffer LightingInfo : register(b0)
{
    float4 materialColor = { 0.7f, 0.7f, 0.0f, .10f };
    float4 ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
    float4 diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    float diffuseIntensity = 2.0f;
    float specularIntensity = 0.6f;
    float attConst = 1.0f;
    float attLin = 0.045f;
    float attQuad = 0.0075f;
    float specPow = 30.0f;
    float3 lightPos = { 0.0f, 0.0f, +2.0f };
};

float4 main(PixelInputType input) : SV_TARGET
{    
    float3 relative = lightPos - (float3) input.worldPosition;
    float dist = length(relative);
    float3 normalizedRelative = relative / dist;
    float att = 1.0f / (attConst + attLin * dist + attQuad * (dist * dist));
    float cosAngle = max(0.0f, dot(normalizedRelative, input.normal));
    float3 diffuse = diffuseColor * diffuseIntensity * cosAngle;
    
    float3 reflected = reflect(normalizedRelative, input.normal);
    
    //TODO- FIX THIS
    float3 toEye = float3(0.0f, 0.0f, 4.0f) - (float3) input.worldPosition;
    
    float specFactor = pow(max(dot(normalize(reflected), normalize(toEye)), 0.0f), specPow);
    
    float3 specular = specularIntensity * specFactor * diffuse ;
    
    return float4(saturate((diffuse + ambientColor +specular )*materialColor*att), 1.0f);
    
}
