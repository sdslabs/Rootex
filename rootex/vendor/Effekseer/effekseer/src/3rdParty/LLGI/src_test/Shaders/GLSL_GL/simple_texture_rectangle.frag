#version 420

struct PS_INPUT
{
    vec4 Position;
    vec2 UV;
    vec4 Color;
};

layout(binding = 0) uniform sampler2D _61;

layout(location = 0) in vec2 input_UV;
layout(location = 1) in vec4 input_Color;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_INPUT _input)
{
    vec4 c = texture(_61, _input.UV);
    c.w = 255.0;
    return c;
}

void main()
{
    PS_INPUT _input;
    _input.Position = gl_FragCoord;
    _input.UV = input_UV;
    _input.Color = input_Color;
    PS_INPUT param = _input;
    _entryPointOutput = _main(param);
}

