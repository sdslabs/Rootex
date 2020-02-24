cbuffer CBuf : register(b0)
{
    float4 color;
};

float4 main() : SV_TARGET
{
    return color;
}
