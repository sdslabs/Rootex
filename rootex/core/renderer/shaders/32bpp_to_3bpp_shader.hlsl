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
    float Palette[8][3]=
    {
        { 0.0f, 0.0f, 0.0f }, 
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 0.0f },
        { 1.0f, 1.0f, 1.0f }
    };
    float m = 999999999.0f;
    float3 closest = { 1.0f, 1.0f, 1.0f };
    float3 curr = float3( r, g, b );
    for (int i = 0; i < 8; i++) {
        float3 tr = float3( Palette[i] );
        float3 error = tr - curr;
        float err = dot( error, error );
        if ( err < m ) {
            m = err;
            closest = tr;
        }
    }

    return float4( closest, 1.0f );
};

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
    float offset = (float(ordered_matrix[(int)(coord.x) & 7][(int)( coord.y ) & 7 ]) + 1 ) / 64.0f - 0.5;
    v = v + offset * 0.4;

    return v;
}


float4 main(DamageVSOutput input) : SV_TARGET
{
    float4 rgbl = InputTexture.Sample(AnisotropicSampler, input.tex);
    float2 uv=float2( input.tex * resolution / 3.0f );
    float r=dithering( uv, rgbl.r );
    float b=dithering( uv, rgbl.b );
    float g=dithering( uv, rgbl.g );
    rgbl = closest( r, g, b );

    return rgbl;
}
