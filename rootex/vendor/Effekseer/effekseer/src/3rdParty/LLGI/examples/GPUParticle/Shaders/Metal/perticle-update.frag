#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_INPUT
{
    float2 UV;
};

struct PS_OUTPUT
{
    float4 PositionAndLocalTime;
    float4 VelocityAndLifeTime;
};

struct main0_out
{
    float4 _entryPointOutput_PositionAndLocalTime [[color(0)]];
    float4 _entryPointOutput_VelocityAndLifeTime [[color(1)]];
};

struct main0_in
{
    float2 input_UV [[user(locn0)]];
};

static inline __attribute__((always_inline))
PS_OUTPUT _main(thread const PS_INPUT& _input, thread texture2d<float> PositionTexture_, thread sampler PositionSamplerState_, thread texture2d<float> VelocityTexture_, thread sampler VelocitySamplerState_)
{
    float4 positionAndLocalTime = PositionTexture_.sample(PositionSamplerState_, _input.UV);
    float4 velocityAndLifeTime = VelocityTexture_.sample(VelocitySamplerState_, _input.UV);
    float3 position = positionAndLocalTime.xyz + velocityAndLifeTime.xyz;
    float localTime = positionAndLocalTime.w + 0.01600000075995922088623046875;
    PS_OUTPUT _output;
    _output.PositionAndLocalTime = float4(position, localTime);
    _output.VelocityAndLifeTime = velocityAndLifeTime;
    return _output;
}

fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> PositionTexture_ [[texture(0)]], texture2d<float> VelocityTexture_ [[texture(1)]], sampler PositionSamplerState_ [[sampler(0)]], sampler VelocitySamplerState_ [[sampler(1)]])
{
    main0_out out = {};
    PS_INPUT _input;
    _input.UV = in.input_UV;
    PS_INPUT param = _input;
    PS_OUTPUT flattenTemp = _main(param, PositionTexture_, PositionSamplerState_, VelocityTexture_, VelocitySamplerState_);
    out._entryPointOutput_PositionAndLocalTime = flattenTemp.PositionAndLocalTime;
    out._entryPointOutput_VelocityAndLifeTime = flattenTemp.VelocityAndLifeTime;
    return out;
}

