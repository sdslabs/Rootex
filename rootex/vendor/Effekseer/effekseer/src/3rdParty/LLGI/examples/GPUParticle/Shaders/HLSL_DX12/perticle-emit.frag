
struct PS_INPUT
{
    float4 PositionData : UV0;
    float4 VelocityAndLifeTimeData : COLOR0;
};

struct VS_OUTPUT
{
    float4 Position : SV_TARGET0;
    float4 VelocityAndLifeTime : SV_TARGET1;
};

VS_OUTPUT main(PS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.PositionData;
    output.VelocityAndLifeTime = input.VelocityAndLifeTimeData;

    // Debug
    //output.Position = float4(1, 0, 0, 0);
    //output.VelocityAndLifeTime = float4(0, 1, 0, 1);
    //output.Position.a = 1.0;

    return output;
}
