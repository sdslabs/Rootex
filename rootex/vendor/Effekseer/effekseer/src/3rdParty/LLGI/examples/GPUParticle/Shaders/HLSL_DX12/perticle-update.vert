
// Note: Copy screen only.

struct VS_INPUT
{
    float3 Position : POSITION0;
    float2 UV : UV0;
};

struct VS_OUTPUT
{
    float2 UV : UV0;
    float4 Position : SV_POSITION;
};
    
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = float4(input.Position, 1.0f);
    output.UV = input.UV;
    return output;
}
