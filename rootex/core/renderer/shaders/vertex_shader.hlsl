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
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.position = mul(mul(P, mul(V, M)), float4(input.position));
    output.tex = input.tex;
    
    return output;
}
