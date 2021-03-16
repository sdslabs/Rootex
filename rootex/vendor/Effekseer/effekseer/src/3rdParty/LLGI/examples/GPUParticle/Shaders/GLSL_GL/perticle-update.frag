#version 420

struct PS_INPUT
{
    vec2 UV;
};

struct PS_OUTPUT
{
    vec4 PositionAndLocalTime;
    vec4 VelocityAndLifeTime;
};

layout(binding = 0) uniform sampler2D _91;
layout(binding = 0) uniform sampler2D _93;

layout(location = 0) in vec2 input_UV;
layout(location = 0) out vec4 _entryPointOutput_PositionAndLocalTime;
layout(location = 1) out vec4 _entryPointOutput_VelocityAndLifeTime;

PS_OUTPUT _main(PS_INPUT _input)
{
    vec4 positionAndLocalTime = texture(_91, _input.UV);
    vec4 velocityAndLifeTime = texture(_93, _input.UV);
    vec3 position = positionAndLocalTime.xyz + velocityAndLifeTime.xyz;
    float localTime = positionAndLocalTime.w + 0.01600000075995922088623046875;
    PS_OUTPUT _output;
    _output.PositionAndLocalTime = vec4(position, localTime);
    _output.VelocityAndLifeTime = velocityAndLifeTime;
    return _output;
}

void main()
{
    PS_INPUT _input;
    _input.UV = input_UV;
    PS_INPUT param = _input;
    PS_OUTPUT flattenTemp = _main(param);
    _entryPointOutput_PositionAndLocalTime = flattenTemp.PositionAndLocalTime;
    _entryPointOutput_VelocityAndLifeTime = flattenTemp.VelocityAndLifeTime;
}

