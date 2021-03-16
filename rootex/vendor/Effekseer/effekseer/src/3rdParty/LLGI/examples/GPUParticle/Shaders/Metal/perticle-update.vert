#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_INPUT
{
    float3 Position;
    float2 UV;
};

struct VS_OUTPUT
{
    float2 UV;
    float4 Position;
};

struct main0_out
{
    float2 _entryPointOutput_UV [[user(locn0)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 input_Position [[attribute(0)]];
    float2 input_UV [[attribute(1)]];
};

static inline __attribute__((always_inline))
VS_OUTPUT _main(thread const VS_INPUT& _input)
{
    VS_OUTPUT _output;
    _output.Position = float4(_input.Position, 1.0);
    _output.UV = _input.UV;
    return _output;
}

vertex main0_out main0(main0_in in [[stage_in]])
{
    main0_out out = {};
    VS_INPUT _input;
    _input.Position = in.input_Position;
    _input.UV = in.input_UV;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    out._entryPointOutput_UV = flattenTemp.UV;
    out.gl_Position = flattenTemp.Position;
    return out;
}

