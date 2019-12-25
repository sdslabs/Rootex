cbuffer CBuf : register(b0)
{
	matrix M;
	matrix V;
    matrix P;
};

cbuffer TCBuf : register(b1)
{
    matrix transform;
};

float4 main(float3 pos: POSITION) : SV_POSITION
{
    matrix T = M;
    T = V * T;
    T = P * T;
    
    return mul(T, float4(pos, 1.0f));
}
