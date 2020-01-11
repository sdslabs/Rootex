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
};

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    matrix M : MATRIX;
    float4 originalPosition : POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.screenPosition = mul(mul(P, mul(V, M)), input.position);
    output.M = M;
    output.originalPosition = input.position;
    output.tex = input.tex;
    
    return output;
}
