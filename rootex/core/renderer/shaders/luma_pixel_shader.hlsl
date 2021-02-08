SamplerState AnisotropicSampler : register(s0);
Texture2D InputTexture : register(t0);

struct LumaVSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(LumaVSOutput input) : SV_TARGET
{
    // https://stackoverflow.com/a/19690356/7370948
    
    float4 rgbl = InputTexture.Sample(AnisotropicSampler, input.tex);
    rgbl.w = (0.2126f * rgbl.r) + (0.7152f * rgbl.g) + (0.0722f * rgbl.b);
    
    return rgbl;
}
