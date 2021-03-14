
Texture2D PositionTexture_ : register(t0);
SamplerState PositionSamplerState_ : register(s0);

Texture2D VelocityTexture_ : register(t1);
SamplerState VelocitySamplerState_ : register(s1);

struct PS_INPUT
{
    float2 UV : UV0;
};

struct PS_OUTPUT
{
    float4 PositionAndLocalTime : SV_TARGET0;
    float4 VelocityAndLifeTime : SV_TARGET1;
};

PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    float4 positionAndLocalTime = PositionTexture_.Sample(PositionSamplerState_, input.UV);
    float4 velocityAndLifeTime = VelocityTexture_.Sample(VelocitySamplerState_, input.UV);

    float3 position = positionAndLocalTime.xyz + velocityAndLifeTime.xyz;
    float localTime = positionAndLocalTime.w + 0.016;

    PS_OUTPUT output;
    output.PositionAndLocalTime = float4(position, localTime);
    output.VelocityAndLifeTime = velocityAndLifeTime;
    return output;
}
