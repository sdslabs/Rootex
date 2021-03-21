cbuffer CB : register(b1)
{
  float4 offset;
};

struct PS_INPUT
{
    float4  Position : SV_POSITION;
	float2  UV : UV0;
    float4  Color    : COLOR0;
};

float4 main(PS_INPUT input) : SV_TARGET 
{ 
	float4 c;
	c = input.Color + offset;
	c.a = 1.0f;
	return c;
}
