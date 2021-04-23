#include "register_locations_pixel_shader.h"

TextureCube SkyTexture : register(SKY_PS_HLSL);
SamplerState SampleType : register(SAMPLER_PS_HLSL);

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{	
    return SkyTexture.Sample(SampleType, input.texCoord);
}
