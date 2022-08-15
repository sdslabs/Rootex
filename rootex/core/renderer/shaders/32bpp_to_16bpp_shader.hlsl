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
    r=round(r*31.)/31;
    b=round(b*31.)/31;
    g=round(g*63.)/63;
	return float4(r,g,b,1.0);

}
float dithering(in float2 coord, inout float v)
{
	float M[64] = {
		0, 48, 12, 60, 3, 51, 15, 63,
		32, 16, 44, 28, 35, 19, 47, 31,
		8, 56, 4, 52, 11, 59, 7, 55,
		40, 24, 36, 20, 43, 27, 39, 23,
		2, 50, 14, 62, 1, 49, 13, 61,
		34, 18, 46, 30, 33, 17, 45, 29,
		10, 58, 6, 54, 9, 57, 5, 53,
		42, 26, 38, 22, 41, 25, 37, 21
	};
    int index = (int((coord.x) % 8.0)+int((coord.y) % 8.)*8);
	float offset=(float(M[index])+1.)/64.-0.5;
	v = v + offset * 0.4;
	return v;
}


float4 main(DamageVSOutput input) : SV_TARGET
{
    float4 rgbl = InputTexture.Sample(AnisotropicSampler, input.tex);
    float2 uv=float2(input.tex*resolution);
    float r=dithering(uv,rgbl.r);
    float b=dithering(uv,rgbl.b);
    float g=dithering(uv,rgbl.g);
	rgbl = closest(r, g, b);
    return rgbl;
}