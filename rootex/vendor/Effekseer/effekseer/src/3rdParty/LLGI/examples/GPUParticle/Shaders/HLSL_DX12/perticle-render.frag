
Texture2D ParticleTexture_ : register(t2);
SamplerState ParticleSamplerState_ : register(s2);

struct PS_INPUT
{
    float2  UV : UV0;
    float4 Color: COLOR0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    if (input.Color.a <= 0.0) discard;

    float4 color = ParticleTexture_.Sample(ParticleSamplerState_, input.UV);


    return color * input.Color;
}
