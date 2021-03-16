#version 420

struct PS_INPUT
{
    vec4 Position;
    vec2 UV;
    vec4 Color;
};

struct PS_OUTPUT
{
    vec4 Color0;
    vec4 Color1;
};

layout(binding = 0) uniform sampler2D _90;

layout(location = 0) in vec2 input_UV;
layout(location = 1) in vec4 input_Color;
layout(location = 0) out vec4 _entryPointOutput_Color0;
layout(location = 1) out vec4 _entryPointOutput_Color1;

PS_OUTPUT _main(PS_INPUT _input)
{
    vec4 c = texture(_90, _input.UV);
    c.w = 255.0;
    PS_OUTPUT _output;
    _output.Color0 = c;
    c.x = 1.0 - c.x;
    c.y = 1.0 - c.y;
    c.z = 1.0 - c.z;
    _output.Color1 = c;
    return _output;
}

void main()
{
    PS_INPUT _input;
    _input.Position = gl_FragCoord;
    _input.UV = input_UV;
    _input.Color = input_Color;
    PS_INPUT param = _input;
    PS_OUTPUT flattenTemp = _main(param);
    _entryPointOutput_Color0 = flattenTemp.Color0;
    _entryPointOutput_Color1 = flattenTemp.Color1;
}

