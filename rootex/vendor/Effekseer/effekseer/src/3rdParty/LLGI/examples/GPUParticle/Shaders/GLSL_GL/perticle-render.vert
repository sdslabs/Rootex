#version 420
#ifdef GL_ARB_shader_draw_parameters
#extension GL_ARB_shader_draw_parameters : enable
#endif

out gl_PerVertex
{
    vec4 gl_Position;
};

struct VS_INPUT
{
    vec3 Position;
    vec2 UV;
    uint InstanceId;
};

struct VS_OUTPUT
{
    vec2 UV;
    vec4 Color;
    vec4 Position;
};

layout(binding = 0, std140) uniform GPUParticleTextureInfo
{
    vec4 TextureResolution;
    layout(row_major) mat4 ViewProjMatrix;
} CBVS0;

layout(binding = 0) uniform sampler2D _143;
layout(binding = 0) uniform sampler2D _145;

layout(location = 0) in vec3 input_Position;
layout(location = 1) in vec2 input_UV;
#ifdef GL_ARB_shader_draw_parameters
#define SPIRV_Cross_BaseInstance gl_BaseInstanceARB
#else
uniform int SPIRV_Cross_BaseInstance;
#endif
layout(location = 0) out vec2 _entryPointOutput_UV;
layout(location = 1) out vec4 _entryPointOutput_Color;

VS_OUTPUT _main(VS_INPUT _input)
{
    vec2 texelPos = vec2(mod(float(_input.InstanceId), CBVS0.TextureResolution.x), float(_input.InstanceId) / CBVS0.TextureResolution.x);
    vec2 fetchUV = texelPos * CBVS0.TextureResolution.zw;
    vec4 positionAndLocalTime = textureLod(_143, fetchUV, 0.0);
    vec4 velocityAndLifeTime = textureLod(_145, fetchUV, 0.0);
    vec3 posOffset = positionAndLocalTime.xyz;
    vec3 worldPos = _input.Position + posOffset;
    VS_OUTPUT _output;
    _output.Position = vec4(worldPos, 1.0) * CBVS0.ViewProjMatrix;
    _output.UV = _input.UV;
    _output.Color = vec4(1.0, 1.0, 1.0, 1.0 - (positionAndLocalTime.w / (velocityAndLifeTime.w + 9.9999997473787516355514526367188e-06)));
    return _output;
}

void main()
{
    VS_INPUT _input;
    _input.Position = input_Position;
    _input.UV = input_UV;
    _input.InstanceId = uint((gl_InstanceID + SPIRV_Cross_BaseInstance));
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_Color = flattenTemp.Color;
    gl_Position = flattenTemp.Position;
}

