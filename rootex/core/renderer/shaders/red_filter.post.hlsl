#include "rootex/core/renderer/shaders/register_locations_pixel_shader.h"

SamplerState SampleType : register(SAMPLER_PS_HLSL);

Texture2D ScreenTexture : register(CUSTOM_TEXTURE_0_PS_HLSL);

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 main(PixelInputType input)
    : SV_TARGET
{
	return float4(ScreenTexture.Sample(SampleType, input.tex).r, 0.0f, 0.0f, 1.0f);
}
