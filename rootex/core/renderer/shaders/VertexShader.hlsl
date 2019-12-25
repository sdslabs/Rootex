cbuffer CBuf : register(b0)
{
	matrix MVP;
};

cbuffer TCBuf : register(b1)
{
    matrix transform;
};

float4 main(float3 pos: POSITION) : SV_POSITION
{
    return mul(float4(pos, 1.0f), MVP);
}
