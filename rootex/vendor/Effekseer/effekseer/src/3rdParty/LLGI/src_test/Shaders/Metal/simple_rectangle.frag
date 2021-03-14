#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_INPUT
{
    float4 g_position;
    float4 g_color;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float4 input_g_color [[user(locn0)]];
};

static inline __attribute__((always_inline))
float4 _main(thread const PS_INPUT& _input)
{
    return _input.g_color;
}

fragment main0_out main0(main0_in in [[stage_in]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_INPUT _input;
    _input.g_position = gl_FragCoord;
    _input.g_color = in.input_g_color;
    PS_INPUT param = _input;
    out._entryPointOutput = _main(param);
    return out;
}

