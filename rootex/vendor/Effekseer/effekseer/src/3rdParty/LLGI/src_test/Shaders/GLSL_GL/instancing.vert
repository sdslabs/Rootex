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
    vec3 g_position;
    vec2 g_uv;
    vec4 g_color;
    uint InstanceId;
};

struct VS_OUTPUT
{
    vec4 g_position;
    vec4 g_color;
};

layout(binding = 0, std140) uniform CB
{
    vec4 offsets[10];
} CBVS0;

layout(location = 0) in vec3 input_g_position;
layout(location = 1) in vec2 input_g_uv;
layout(location = 2) in vec4 input_g_color;
#ifdef GL_ARB_shader_draw_parameters
#define SPIRV_Cross_BaseInstance gl_BaseInstanceARB
#else
uniform int SPIRV_Cross_BaseInstance;
#endif
layout(location = 0) out vec4 _entryPointOutput_g_color;

VS_OUTPUT _main(VS_INPUT _input)
{
    VS_OUTPUT _output;
    _output.g_position = vec4(_input.g_position, 1.0);
    _output.g_position.x += CBVS0.offsets[_input.InstanceId].x;
    _output.g_position.y += CBVS0.offsets[_input.InstanceId].y;
    _output.g_color = _input.g_color;
    return _output;
}

void main()
{
    VS_INPUT _input;
    _input.g_position = input_g_position;
    _input.g_uv = input_g_uv;
    _input.g_color = input_g_color;
    _input.InstanceId = uint((gl_InstanceID + SPIRV_Cross_BaseInstance));
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    gl_Position = flattenTemp.g_position;
    _entryPointOutput_g_color = flattenTemp.g_color;
}

