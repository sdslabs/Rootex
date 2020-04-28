cbuffer CBuf : register(b3)
{
    float4 color;
};

float4 main() : SV_TARGET
{
    return color;
}
