#version 420

struct PS_INPUT
{
    vec4 g_position;
    vec4 g_color;
};

layout(location = 0) in vec4 input_g_color;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_INPUT _input)
{
    return _input.g_color;
}

void main()
{
    PS_INPUT _input;
    _input.g_position = gl_FragCoord;
    _input.g_color = input_g_color;
    PS_INPUT param = _input;
    _entryPointOutput = _main(param);
}

