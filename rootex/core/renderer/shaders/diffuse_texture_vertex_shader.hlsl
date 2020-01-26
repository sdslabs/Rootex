cbuffer CBuf : register(b0)
{
    matrix M;
};

cbuffer CBuf : register(b1)
{
    matrix V;
};

cbuffer CBuf : register(b2)
{
    matrix P;
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
    matrix M : MATRIX;
    float4 worldPosition : POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.screenPosition = mul(mul(P, mul(V, M)), input.position);
    output.normal = mul((float3x3) M, (float3)input.normal);
    output.M = M;
    output.worldPosition = mul(M, input.position);
    output.tex = input.tex;
    
    return output;
}
