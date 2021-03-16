#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_INPUT
{
    float4 Position;
    float2 UV;
    float4 Color;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float2 input_UV [[user(locn0)]];
    float4 input_Color [[user(locn1)]];
};

static inline __attribute__((always_inline))
float4 _main(thread const PS_INPUT& _input, thread texture2d<float> txt, thread sampler smp)
{
    float4 c = txt.sample(smp, _input.UV);
    c.w = 255.0;
    return c;
}

fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> txt [[texture(0)]], sampler smp [[sampler(0)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_INPUT _input;
    _input.Position = gl_FragCoord;
    _input.UV = in.input_UV;
    _input.Color = in.input_Color;
    PS_INPUT param = _input;
    out._entryPointOutput = _main(param, txt, smp);
    return out;
}

