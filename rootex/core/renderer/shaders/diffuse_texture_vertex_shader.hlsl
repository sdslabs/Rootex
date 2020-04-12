cbuffer CBuf : register(b1)
{
    matrix M;
};

cbuffer CBuf : register(b2)
{
    matrix V;
};

cbuffer CBuf : register(b3)
{
    matrix P;
};

cbuffer CBuf : register(b4)
{
    matrix MInverse;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
};

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float4 worldPosition : POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.screenPosition = mul(input.position, mul(M, mul(V, P)));
    //inverse transpose is needed for normals, how is this even working...
    //output.normal = mul((float3x3) M, (float3) input.normal);
    //hold my beer...
    output.normal = mul((float3x3) transpose(MInverse), (float3) input.normal);
    output.worldPosition = mul(input.position, M);
    output.tex.x = input.tex.x;
    output.tex.y = 1 - input.tex.y;
    input.normal = normalize(input.normal);
    
    return output;
}
