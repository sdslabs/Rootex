#version 420

struct VS_INPUT
{
    vec2 ParticleIDAndLifeTime;
    vec3 Position;
    vec3 Velocity;
};

struct VS_OUTPUT
{
    vec4 PositionData;
    vec4 VelocityAndLifeTimeData;
    vec4 Position;
};

layout(set = 0, binding = 0, std140) uniform GPUParticleTextureInfo
{
    vec4 TextureSize;
    layout(row_major) mat4 ViewProjMatrix;
} _23;

layout(location = 0) in vec2 input_ParticleIDAndLifeTime;
layout(location = 1) in vec3 input_Position;
layout(location = 2) in vec3 input_Velocity;
layout(location = 0) out vec4 _entryPointOutput_PositionData;
layout(location = 1) out vec4 _entryPointOutput_VelocityAndLifeTimeData;

VS_OUTPUT _main(VS_INPUT _input)
{
    vec2 HalfPixelOffsetInClipSpace = vec2(1.0 / _23.TextureSize.x, (-1.0) / _23.TextureSize.y);
    float particleID = _input.ParticleIDAndLifeTime.x;
    float lifeTime = _input.ParticleIDAndLifeTime.y;
    vec2 texelPos = vec2(mod(particleID, _23.TextureSize.x), particleID / _23.TextureSize.x);
    vec2 svPos = (texelPos / vec2(_23.TextureSize.xy)) * 2.0;
    svPos += vec2(-1.0);
    svPos.y *= (-1.0);
    svPos += HalfPixelOffsetInClipSpace;
    VS_OUTPUT _output;
    _output.Position = vec4(svPos, 0.0, 1.0);
    _output.PositionData = vec4(_input.Position, 0.0);
    _output.VelocityAndLifeTimeData = vec4(_input.Velocity, lifeTime);
    return _output;
}

void main()
{
    VS_INPUT _input;
    _input.ParticleIDAndLifeTime = input_ParticleIDAndLifeTime;
    _input.Position = input_Position;
    _input.Velocity = input_Velocity;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    _entryPointOutput_PositionData = flattenTemp.PositionData;
    _entryPointOutput_VelocityAndLifeTimeData = flattenTemp.VelocityAndLifeTimeData;
    vec4 _position = flattenTemp.Position;
    _position.y = -_position.y;
    gl_Position = _position;
}

