Texture2D ShaderTexture;
SamplerState SampleType;

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    matrix M : MATRIX;
    float4 originalPosition : POSITION;
    float2 tex : TEXCOORD0;
};

const float3 materialColor = { 0.7f, 0.7f, 0.9f };
const float3 ambient = { 0.05f, 0.05f, 0.05f };
const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
const float diffuseIntensity = 1.0f;
const float attConst = 1.0f;
const float attLin = 0.045f;
const float attQuad = 0.0075f;

float4 main(PixelInputType input) : SV_TARGET
{/*
    float3 relative = - input.originalPosition;
    float dist = length(relative);
    float3 normalizedRelative = relative / dist;
    float att = 1.0f / (attConst + attLin * dist + attQuad * (dist * dist));
    //float3 diffuse = ShaderTexture.Sample(SampleType, input.tex);
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(relative, input.normal));
    //diffuse = diffuse * diffuseIntensity * att * max(0.0f, dot(relative, input.normal));
    
    float4 res = float4(saturate(diffuse + ambient), 1.0f);
    
    return float4(ambient, 1.0f);*/
    
    float3 relative = (float3) mul(input.M, input.originalPosition);
    float dist = length(relative);
    float3 normalizedRelative = relative / dist;
    float att = 1.0f / (1.0f + 0.045f * dist + 0.0075f * (dist * dist));
    //float3 diffuse = ShaderTexture.Sample(SampleType, input.tex);
    float3 diffuse = float3(1.0f, 1.0f, 1.0f) * 3.0f * att * max(0.0f, dot(normalizedRelative, input.normal));
    //diffuse = diffuse * diffuseIntensity * att * max(0.0f, dot(relative, input.normal));
    
    float4 res = float4(saturate(diffuse + float3(0.05f, 0.05f,0.05f)), 1.0f);
    
    return res;
    
    //float4 textureColor;
    
    //textureColor = float4(0.2f, 0.2f, 0.0f, 1.0f) + ShaderTexture.Sample(SampleType, input.tex) / distance(mul(input.M, input.originalPosition).xyz, float3(0.0f, 0.0f, 2.0f));
    
	//return textureColor;
}
