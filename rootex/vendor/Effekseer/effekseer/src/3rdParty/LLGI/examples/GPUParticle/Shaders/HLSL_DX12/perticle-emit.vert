struct VS_INPUT
{
    float2 ParticleIDAndLifeTime: UV0;
    float3 Position: POSITION0;
    float3 Velocity: COLOR0;
};

struct VS_OUTPUT
{
    float4 PositionData : UV0;
    float4 VelocityAndLifeTimeData : COLOR0;
    float4 Position : SV_POSITION;
};

cbuffer GPUParticleTextureInfo : register(b0)
{
    float4 TextureSize;
    float4x4 ViewProjMatrix;
};


VS_OUTPUT main(VS_INPUT input)
{
    //const float2 TextureSize2 = float2(512, 512);
    const float2 HalfPixelOffsetInClipSpace = float2(1.0 / TextureSize.x, -1.0 / TextureSize.y);

    VS_OUTPUT output;
    float particleID = input.ParticleIDAndLifeTime.x;
    float lifeTime = input.ParticleIDAndLifeTime.y;
    
    //float2 svPos = float2(particleID & ID2TPos.x, particleID >> ID2TPos.y) * TPos2VPos.x + TPos2VPos.y;
    float2 texelPos = float2(fmod(particleID, TextureSize.x), particleID / TextureSize.x);
    float2 svPos = (texelPos / TextureSize) * 2.0;
    svPos += float2(-1.0, -1.0);
    svPos.y *= -1.0;

    // "GeForce RTX 2080 SUPER" + DX12 にて、(-1.0, 1.0) のように境界の位置にレンダリングしようとしたとき、描画されなくなることがあった。
    // そのため、確実に Pixel 内にレンダリングされるようにオフセットをつけている。
    svPos += HalfPixelOffsetInClipSpace;

    //svPos = float2(particleID / 10.0, 0);

    output.Position = float4(svPos, 0.0, 1.0);
    output.PositionData = float4(input.Position, 0.0);
    output.VelocityAndLifeTimeData = float4(input.Velocity, lifeTime);
    
    return output;
}
