#version 420

struct PS_INPUT
{
    vec4 PositionData;
    vec4 VelocityAndLifeTimeData;
};

struct VS_OUTPUT
{
    vec4 Position;
    vec4 VelocityAndLifeTime;
};

layout(location = 0) in vec4 input_PositionData;
layout(location = 1) in vec4 input_VelocityAndLifeTimeData;
layout(location = 0) out vec4 _entryPointOutput_Position;
layout(location = 1) out vec4 _entryPointOutput_VelocityAndLifeTime;

VS_OUTPUT _main(PS_INPUT _input)
{
    VS_OUTPUT _output;
    _output.Position = _input.PositionData;
    _output.VelocityAndLifeTime = _input.VelocityAndLifeTimeData;
    return _output;
}

void main()
{
    PS_INPUT _input;
    _input.PositionData = input_PositionData;
    _input.VelocityAndLifeTimeData = input_VelocityAndLifeTimeData;
    PS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    _entryPointOutput_Position = flattenTemp.Position;
    _entryPointOutput_VelocityAndLifeTime = flattenTemp.VelocityAndLifeTime;
}

