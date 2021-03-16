#version 420

struct VS_INPUT
{
    vec3 Position;
    vec2 UV;
    vec4 Color;
};

struct VS_OUTPUT
{
    vec4 Position;
    vec2 UV;
    vec4 Color;
};

layout(set = 0, binding = 0, std140) uniform CB
{
    vec4 offset;
} _31;

layout(location = 0) in vec3 input_Position;
layout(location = 1) in vec2 input_UV;
layout(location = 2) in vec4 input_Color;
layout(location = 0) out vec2 _entryPointOutput_UV;
layout(location = 1) out vec4 _entryPointOutput_Color;

VS_OUTPUT _main(VS_INPUT _input)
{
    VS_OUTPUT _output;
    _output.Position = vec4(_input.Position, 1.0) + _31.offset;
    _output.UV = _input.UV;
    _output.Color = _input.Color;
    return _output;
}

void main()
{
    VS_INPUT _input;
    _input.Position = input_Position;
    _input.UV = input_UV;
    _input.Color = input_Color;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    vec4 _position = flattenTemp.Position;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_Color = flattenTemp.Color;
}

