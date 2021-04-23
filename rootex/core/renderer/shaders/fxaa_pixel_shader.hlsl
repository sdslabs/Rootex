#define FXAA_PC 1
#define FXAA_QUALITY__PRESET 15
#define FXAA_HLSL_4 1
#include "rootex/vendor/FXAA/FXAA3_11.h"

cbuffer cbFxaa : register(b0)
{
    float4 rcpFrame;
};

SamplerState AnisotropicSampler : register(s0);
Texture2D InputTexture : register(t0);

struct FxaaVSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(FxaaVSOutput input) : SV_TARGET
{
    float4 dummy = float4(0.0f, 0.0f, 0.0f, 0.0f);
    FxaaTex tex = { AnisotropicSampler, InputTexture };
    
    float4 postFXAAColor = FxaaPixelShader(
        input.tex,
        dummy,
        tex,
        tex,
        tex,
        rcpFrame.xy,
        dummy,
        dummy,
        dummy,
        0.75f,
        0.166f,
        0.0625f,
        dummy.x,
        dummy.x,
        dummy.x,
        dummy);
    postFXAAColor.w = 1.0f;
    
    return postFXAAColor;
}
