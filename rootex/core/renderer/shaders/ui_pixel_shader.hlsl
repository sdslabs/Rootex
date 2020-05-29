#include "register_locations_pixel_shader.h"

Texture2D ShaderTexture;
SamplerState SampleType;

struct UIPixelInput
{
    float4 screenPosition : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
};

float4 main(UIPixelInput input) : SV_TARGET
{
    float4 color = ShaderTexture.Sample(SampleType, input.tex) * input.color;
	
	return color;
}
