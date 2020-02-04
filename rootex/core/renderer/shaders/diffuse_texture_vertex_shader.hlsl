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
    output.screenPosition = mul(input.position, mul(M, mul(V, P)));
    //inverse transpose is needed for normals, how is this even working...
    output.normal = mul((float3x3) M, (float3)input.normal);
    output.M = M;
    output.worldPosition = mul(input.position, M);
    output.tex = input.tex;
    
    return output;
}
