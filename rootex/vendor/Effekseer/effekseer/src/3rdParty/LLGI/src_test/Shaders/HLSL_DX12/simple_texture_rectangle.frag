Texture2D txt : register(t0);
SamplerState smp : register(s0);

struct PS_INPUT
{
    float4  Position : SV_POSITION;
	float2  UV : UV0;
    float4  Color    : COLOR0;
};

float4 main(PS_INPUT input) : SV_TARGET 
{ 
	float4 c;
	c = txt.Sample(smp, input.UV);
	c.a = 255;
	return c;
}
