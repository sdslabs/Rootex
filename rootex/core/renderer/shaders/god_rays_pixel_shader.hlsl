cbuffer cbGodRays : register(b0)
{
	float3 sunScreenSpacePos;
	int numSamples;
	float density;
	float weight;
	float decay;
	float exposure;
};

SamplerState SampleType : register(s0);
Texture2D InputTexture : register(t0);

struct GodRaysVSOutput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 main(GodRaysVSOutput input) : SV_TARGET
{
	float4 rgba = InputTexture.Sample(SampleType, input.tex);
	float lum = (0.2126f * rgba.r) + (0.7152f * rgba.g) + (0.0722f * rgba.b);

	return float4(lum, lum, lum, rgba.a);
}