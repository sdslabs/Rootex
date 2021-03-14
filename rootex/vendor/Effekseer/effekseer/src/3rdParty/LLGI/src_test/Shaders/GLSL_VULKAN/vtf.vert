#version 420

struct VS_INPUT
{
    vec3 g_position;
    vec2 g_uv;
    vec4 g_color;
};

struct VS_OUTPUT
{
    vec4 g_position;
    vec4 g_color;
};

layout(set = 0, binding = 1) uniform sampler2D Sampler_smp;

layout(location = 0) in vec3 input_g_position;
layout(location = 1) in vec2 input_g_uv;
layout(location = 2) in vec4 input_g_color;
layout(location = 0) out vec4 _entryPointOutput_g_color;

VS_OUTPUT _main(VS_INPUT _input)
{
    vec4 c = textureLod(Sampler_smp, _input.g_uv, 0.0);
    VS_OUTPUT _output;
    _output.g_position = vec4(_input.g_position, 1.0);
    vec2 _53 = _output.g_position.xy + c.xy;
    _output.g_position = vec4(_53.x, _53.y, _output.g_position.z, _output.g_position.w);
    _output.g_color = _input.g_color;
    return _output;
}

void main()
{
    VS_INPUT _input;
    _input.g_position = input_g_position;
    _input.g_uv = input_g_uv;
    _input.g_color = input_g_color;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    vec4 _position = flattenTemp.g_position;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_g_color = flattenTemp.g_color;
}

