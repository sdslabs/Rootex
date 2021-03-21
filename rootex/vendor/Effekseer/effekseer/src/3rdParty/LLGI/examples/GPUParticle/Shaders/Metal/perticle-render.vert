#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_INPUT
{
    float3 Position;
    float2 UV;
    uint InstanceId;
};

struct VS_OUTPUT
{
    float2 UV;
    float4 Color;
    float4 Position;
};

struct GPUParticleTextureInfo
{
    float4 TextureResolution;
    float4x4 ViewProjMatrix;
};

struct main0_out
{
    float2 _entryPointOutput_UV [[user(locn0)]];
    float4 _entryPointOutput_Color [[user(locn1)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 input_Position [[attribute(0)]];
    float2 input_UV [[attribute(1)]];
};

// Implementation of the GLSL mod() function, which is slightly different than Metal fmod()
template<typename Tx, typename Ty>
inline Tx mod(Tx x, Ty y)
{
    return x - y * floor(x / y);
}

static inline __attribute__((always_inline))
VS_OUTPUT _main(thread const VS_INPUT& _input, constant GPUParticleTextureInfo& v_29, thread texture2d<float> PositionTexture_, thread sampler PositionSamplerState_, thread texture2d<float> VelocityTexture_, thread sampler VelocitySamplerState_)
{
    float2 texelPos = float2(mod(float(_input.InstanceId), v_29.TextureResolution.x), float(_input.InstanceId) / v_29.TextureResolution.x);
    float2 fetchUV = texelPos * v_29.TextureResolution.zw;
    float4 positionAndLocalTime = PositionTexture_.sample(PositionSamplerState_, fetchUV, level(0.0));
    float4 velocityAndLifeTime = VelocityTexture_.sample(VelocitySamplerState_, fetchUV, level(0.0));
    float3 posOffset = positionAndLocalTime.xyz;
    float3 worldPos = _input.Position + posOffset;
    VS_OUTPUT _output;
    _output.Position = v_29.ViewProjMatrix * float4(worldPos, 1.0);
    _output.UV = _input.UV;
    _output.Color = float4(1.0, 1.0, 1.0, 1.0 - (positionAndLocalTime.w / (velocityAndLifeTime.w + 9.9999997473787516355514526367188e-06)));
    return _output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant GPUParticleTextureInfo& v_29 [[buffer(0)]], texture2d<float> PositionTexture_ [[texture(0)]], texture2d<float> VelocityTexture_ [[texture(1)]], sampler PositionSamplerState_ [[sampler(0)]], sampler VelocitySamplerState_ [[sampler(1)]], uint gl_InstanceIndex [[instance_id]])
{
    main0_out out = {};
    VS_INPUT _input;
    _input.Position = in.input_Position;
    _input.UV = in.input_UV;
    _input.InstanceId = gl_InstanceIndex;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param, v_29, PositionTexture_, PositionSamplerState_, VelocityTexture_, VelocitySamplerState_);
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_Color = flattenTemp.Color;
    out.gl_Position = flattenTemp.Position;
    return out;
}

