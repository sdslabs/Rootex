#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_INPUT
{
    float3 g_position;
    float2 g_uv;
    float4 g_color;
};

struct VS_OUTPUT
{
    float4 g_position;
    float4 g_color;
};

struct main0_out
{
    float4 _entryPointOutput_g_color [[user(locn0)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 input_g_position [[attribute(0)]];
    float2 input_g_uv [[attribute(1)]];
    float4 input_g_color [[attribute(2)]];
};

static inline __attribute__((always_inline))
VS_OUTPUT _main(thread const VS_INPUT& _input, thread texture2d<float> txt, thread sampler smp)
{
    float4 c = txt.sample(smp, _input.g_uv, level(0.0));
    VS_OUTPUT _output;
    _output.g_position = float4(_input.g_position, 1.0);
    float2 _53 = _output.g_position.xy + c.xy;
    _output.g_position = float4(_53.x, _53.y, _output.g_position.z, _output.g_position.w);
    _output.g_color = _input.g_color;
    return _output;
}

vertex main0_out main0(main0_in in [[stage_in]], texture2d<float> txt [[texture(0)]], sampler smp [[sampler(0)]])
{
    main0_out out = {};
    VS_INPUT _input;
    _input.g_position = in.input_g_position;
    _input.g_uv = in.input_g_uv;
    _input.g_color = in.input_g_color;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param, txt, smp);
    out.gl_Position = flattenTemp.g_position;
    out._entryPointOutput_g_color = flattenTemp.g_color;
    return out;
}

