Texture2D ShaderTexture;
SamplerState SampleType;

cbuffer CBuf
{
	float4 face_colors[6];
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    
    textureColor = ShaderTexture.Sample(SampleType, input.tex);
    
	return textureColor;
}
