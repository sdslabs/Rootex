#include "register_locations_pixel_shader.h"
Texture2D InputTexture : register(t0);
SamplerState AnisotropicSampler : register(s0);

cbuffer PerFrame : register(CUSTOM_PER_FRAME_PS_HLSL)
{
    float timeMs;
    float deltaTimeMs;
    float2 resolution;
    float2 mouse;
};

struct DamageVSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 closest(float r,float g, float b) {
    r=round( r * 31. ) / 31.;
    b=round( b * 31. ) / 31.;
    g=round( g * 63. ) / 63.;

    return float4( r, g, b, 1. );

}

float dithering(in float2 coord, inout float v)
{
    int ordered_matrix[8][8] = {
        { 0, 32, 8, 40, 2, 34, 10, 42 },
        { 48, 16, 56, 24, 50, 18, 58, 26 },
        { 12, 44, 4, 36, 14, 46, 6, 38 },
        { 60, 28, 52, 20, 62, 30, 54, 22 },
        { 3, 35, 11, 43, 1, 33, 9, 41 },
        { 51, 19, 59, 27, 49, 17, 57, 25 },
        { 15, 47, 7, 39, 13, 45, 5, 37 },
        { 63, 31, 55, 23, 61, 29, 53, 21 }

    };
    float offset = (float(ordered_matrix[(int)(coord.x) & 7][(int)(coord.y) & 7] ) + 1 ) / 64. - 0.5;
    v = v + offset * 0.4;

    return v;
}


float4 main(DamageVSOutput input) : SV_TARGET
{
    float4 rgbl = InputTexture.Sample(AnisotropicSampler, input.tex);
    float2 uv=float2( input.tex * resolution / 3. );
    float r=dithering( uv, rgbl.r );
    float b=dithering( uv, rgbl.b );
    float g=dithering( uv, rgbl.g );
    rgbl = closest( r, g, b );
    
    return rgbl;
} 
