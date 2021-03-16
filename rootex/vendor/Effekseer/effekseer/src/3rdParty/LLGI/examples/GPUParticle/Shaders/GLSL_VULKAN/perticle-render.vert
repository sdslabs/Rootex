#version 420

struct VS_INPUT
{
    vec3 Position;
    vec2 UV;
    uint InstanceId;
};

struct VS_OUTPUT
{
    vec2 UV;
    vec4 Color;
    vec4 Position;
};

layout(set = 0, binding = 0, std140) uniform GPUParticleTextureInfo
{
    vec4 TextureResolution;
    layout(row_major) mat4 ViewProjMatrix;
} _29;

layout(set = 0, binding = 1) uniform sampler2D Sampler_PositionSamplerState_;
layout(set = 0, binding = 2) uniform sampler2D Sampler_VelocitySamplerState_;

layout(location = 0) in vec3 input_Position;
layout(location = 1) in vec2 input_UV;
layout(location = 0) out vec2 _entryPointOutput_UV;
layout(location = 1) out vec4 _entryPointOutput_Color;

VS_OUTPUT _main(VS_INPUT _input)
{
    vec2 texelPos = vec2(mod(float(_input.InstanceId), _29.TextureResolution.x), float(_input.InstanceId) / _29.TextureResolution.x);
    vec2 fetchUV = texelPos * _29.TextureResolution.zw;
    vec4 positionAndLocalTime = textureLod(Sampler_PositionSamplerState_, fetchUV, 0.0);
    vec4 velocityAndLifeTime = textureLod(Sampler_VelocitySamplerState_, fetchUV, 0.0);
    vec3 posOffset = positionAndLocalTime.xyz;
    vec3 worldPos = _input.Position + posOffset;
    VS_OUTPUT _output;
    _output.Position = vec4(worldPos, 1.0) * _29.ViewProjMatrix;
    _output.UV = _input.UV;
    _output.Color = vec4(1.0, 1.0, 1.0, 1.0 - (positionAndLocalTime.w / (velocityAndLifeTime.w + 9.9999997473787516355514526367188e-06)));
    return _output;
}

void main()
{
    VS_INPUT _input;
    _input.Position = input_Position;
    _input.UV = input_UV;
    _input.InstanceId = uint(gl_InstanceIndex);
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_Color = flattenTemp.Color;
    vec4 _position = flattenTemp.Position;
    _position.y = -_position.y;
    gl_Position = _position;
}

