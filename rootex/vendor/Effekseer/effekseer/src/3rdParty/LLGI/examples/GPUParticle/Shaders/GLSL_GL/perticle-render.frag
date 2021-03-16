#version 420

struct PS_INPUT
{
    vec2 UV;
    vec4 Color;
};

layout(binding = 0) uniform sampler2D _66;

layout(location = 0) in vec2 input_UV;
layout(location = 1) in vec4 input_Color;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_INPUT _input)
{
    if (_input.Color.w <= 0.0)
    {
        discard;
    }
    vec4 color = texture(_66, _input.UV);
    return color * _input.Color;
}

void main()
{
    PS_INPUT _input;
    _input.UV = input_UV;
    _input.Color = input_Color;
    PS_INPUT param = _input;
    vec4 _64 = _main(param);
    _entryPointOutput = _64;
}

