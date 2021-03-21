
Texture2D PositionTexture_ : register(t0);
SamplerState PositionSamplerState_ : register(s0);

Texture2D VelocityTexture_ : register(t1);
SamplerState VelocitySamplerState_ : register(s1);

struct VS_INPUT
{
    float3 Position : POSITION0;
    float2 UV : UV0;
    uint InstanceId : SV_InstanceID;    // = ParticleId
};

struct VS_OUTPUT
{
    float2 UV : UV0;
    float4 Color: COLOR0;
    float4 Position : SV_POSITION;
};

cbuffer GPUParticleTextureInfo : register(b0)
{
    float4 TextureResolution;
    float4x4 ViewProjMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
    float2 texelPos = float2(fmod(input.InstanceId, TextureResolution.x), input.InstanceId / TextureResolution.x);
    float2 fetchUV = texelPos * TextureResolution.zw;
    float4 positionAndLocalTime = PositionTexture_.SampleLevel(PositionSamplerState_, fetchUV, 0);
    float4 velocityAndLifeTime = VelocityTexture_.SampleLevel(VelocitySamplerState_, fetchUV, 0);

    float3 posOffset = positionAndLocalTime.xyz;

    float3 worldPos = input.Position + posOffset;

    VS_OUTPUT output;
    output.Position = mul(ViewProjMatrix, float4(worldPos, 1.0f));
    output.UV = input.UV;
    output.Color = float4(1, 1, 1, 1.0f - (positionAndLocalTime.w / (velocityAndLifeTime.w + 0.00001)));

    return output;
}
