#define THRESHOLD 0.95f

cbuffer cbGodRays : register(b0)
{
	float3 sunScreenSpacePos;
	int numSamples;
	float density;
	float weight;
	float decay;
	float exposure;
	float2 screenDims;
};

SamplerState SampleType : register(s0);

Texture2D InputTexture : register(t0);
Texture2D<uint2> StencilTexture : register(t1);

struct GodRaysVSOutput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 SelectiveSample(float2 texCoord)
{
	uint3 index = uint3(int2(screenDims * texCoord), 0);
	uint stencil = StencilTexture.Load(index).g;

	return InputTexture.Sample(SampleType, texCoord) * (1 - stencil);
}

float4 main(GodRaysVSOutput input) : SV_TARGET
{
	float2 texCoord = input.tex.xy;

	float2 deltaTexCoord = input.tex - sunScreenSpacePos.xy;
	deltaTexCoord *= (1.0f / float(numSamples)) * density;

	// float3 color = smoothstep(THRESHOLD, 1.0f, SelectiveSample(texCoord).xyz);
	float3 color = SelectiveSample(texCoord).xyz;

	float illumination = 1.0f;

	for (int i = 0; i < numSamples; i++)
	{
		texCoord -= deltaTexCoord;
		// float3 smpl = smoothstep(THRESHOLD, 1.0f, SelectiveSample(texCoord).xyz);
		float3 smpl = SelectiveSample(texCoord).xyz;
		smpl *= illumination * weight;
		color += smpl;
		illumination *= decay;
	}

	// float lum = color.g * exposure - 0.2f;

	float4 origColor = InputTexture.Sample(SampleType, input.tex);
	float4 raysColor = float4(color * exposure, 1.0f);

	float4 finalColor = lerp(origColor, raysColor, 0.1f);

	return float4(finalColor.xyz, 1.0f);
}
