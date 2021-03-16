#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_INPUT
{
    float2 ParticleIDAndLifeTime;
    float3 Position;
    float3 Velocity;
};

struct VS_OUTPUT
{
    float4 PositionData;
    float4 VelocityAndLifeTimeData;
    float4 Position;
};

struct GPUParticleTextureInfo
{
    float4 TextureSize;
    float4x4 ViewProjMatrix;
};

struct main0_out
{
    float4 _entryPointOutput_PositionData [[user(locn0)]];
    float4 _entryPointOutput_VelocityAndLifeTimeData [[user(locn1)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float2 input_ParticleIDAndLifeTime [[attribute(0)]];
    float3 input_Position [[attribute(1)]];
    float3 input_Velocity [[attribute(2)]];
};

// Implementation of the GLSL mod() function, which is slightly different than Metal fmod()
template<typename Tx, typename Ty>
inline Tx mod(Tx x, Ty y)
{
    return x - y * floor(x / y);
}

static inline __attribute__((always_inline))
VS_OUTPUT _main(thread const VS_INPUT& _input, constant GPUParticleTextureInfo& v_23)
{
    float2 HalfPixelOffsetInClipSpace = float2(1.0 / v_23.TextureSize.x, (-1.0) / v_23.TextureSize.y);
    float particleID = _input.ParticleIDAndLifeTime.x;
    float lifeTime = _input.ParticleIDAndLifeTime.y;
    float2 texelPos = float2(mod(particleID, v_23.TextureSize.x), particleID / v_23.TextureSize.x);
    float2 svPos = (texelPos / float2(v_23.TextureSize.xy)) * 2.0;
    svPos += float2(-1.0);
    svPos.y *= (-1.0);
    svPos += HalfPixelOffsetInClipSpace;
    VS_OUTPUT _output;
    _output.Position = float4(svPos, 0.0, 1.0);
    _output.PositionData = float4(_input.Position, 0.0);
    _output.VelocityAndLifeTimeData = float4(_input.Velocity, lifeTime);
    return _output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant GPUParticleTextureInfo& v_23 [[buffer(0)]])
{
    main0_out out = {};
    VS_INPUT _input;
    _input.ParticleIDAndLifeTime = in.input_ParticleIDAndLifeTime;
    _input.Position = in.input_Position;
    _input.Velocity = in.input_Velocity;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param, v_23);
    out._entryPointOutput_PositionData = flattenTemp.PositionData;
    out._entryPointOutput_VelocityAndLifeTimeData = flattenTemp.VelocityAndLifeTimeData;
    out.gl_Position = flattenTemp.Position;
    return out;
}

