#include "register_locations_pixel_shader.h"
Texture2D InputTexture : register(t0);
SamplerState AnisotropicSampler : register(s0);

struct DamageVSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(DamageVSOutput input) : SV_TARGET
{
    float4 rgbl = InputTexture.Sample(AnisotropicSampler, input.tex);
    rgbl = ( float4( rgbl.r, rgbl.g, rgbl.b, 1. ) );
    
    return rgbl;
} 
