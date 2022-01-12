#define THRESHOLD 0.95f

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
	float2 texCoord = input.tex.xy;

	float2 deltaTexCoord = input.tex - sunScreenSpacePos.xy;
	deltaTexCoord *= (1.0f / float(numSamples)) * density;

	float3 color = smoothstep(THRESHOLD, 1.0f, InputTexture.Sample(SampleType, texCoord).xyz);

	float illumination = 1.0f;

	for (int i = 0; i < numSamples; i++)
	{
		texCoord -= deltaTexCoord;
		float4 rawSmpl = InputTexture.Sample(SampleType, texCoord);
		float3 smpl = smoothstep(THRESHOLD, 1.0f, rawSmpl.xyz);
		smpl *= illumination * weight;
		color += smpl;
		illumination *= decay;
	}

	float lum = color.g * exposure - 0.2f;

	float4 origColor = InputTexture.Sample(SampleType, input.tex);
	float4 raysColor = float4(lum, lum, lum, 1.0f);

	float4 finalColor = lerp(origColor, raysColor, 0.1f);

	return float4(finalColor.xyz, 1.0);
}