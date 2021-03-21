#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_INPUT
{
    float4 PositionData;
    float4 VelocityAndLifeTimeData;
};

struct VS_OUTPUT
{
    float4 Position;
    float4 VelocityAndLifeTime;
};

struct main0_out
{
    float4 _entryPointOutput_Position [[color(0)]];
    float4 _entryPointOutput_VelocityAndLifeTime [[color(1)]];
};

struct main0_in
{
    float4 input_PositionData [[user(locn0)]];
    float4 input_VelocityAndLifeTimeData [[user(locn1)]];
};

static inline __attribute__((always_inline))
VS_OUTPUT _main(thread const PS_INPUT& _input)
{
    VS_OUTPUT _output;
    _output.Position = _input.PositionData;
    _output.VelocityAndLifeTime = _input.VelocityAndLifeTimeData;
    return _output;
}

fragment main0_out main0(main0_in in [[stage_in]])
{
    main0_out out = {};
    PS_INPUT _input;
    _input.PositionData = in.input_PositionData;
    _input.VelocityAndLifeTimeData = in.input_VelocityAndLifeTimeData;
    PS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    out._entryPointOutput_Position = flattenTemp.Position;
    out._entryPointOutput_VelocityAndLifeTime = flattenTemp.VelocityAndLifeTime;
    return out;
}

