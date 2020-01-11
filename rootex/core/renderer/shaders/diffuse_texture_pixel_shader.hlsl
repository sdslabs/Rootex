Texture2D ShaderTexture;
SamplerState SampleType;

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    matrix M : MATRIX;
    float4 originalPosition : POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    
    textureColor = float4(0.2f, 0.2f, 0.0f, 1.0f) + ShaderTexture.Sample(SampleType, input.tex) / distance(mul(input.M, input.originalPosition).xyz, float3(0.0f, 0.0f, 2.0f));
    
	return textureColor;
}
