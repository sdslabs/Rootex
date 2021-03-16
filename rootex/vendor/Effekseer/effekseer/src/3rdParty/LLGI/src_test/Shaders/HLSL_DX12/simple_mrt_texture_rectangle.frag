Texture2D txt : register(t0);
SamplerState smp : register(s0);

struct PS_INPUT
{
    float4  Position : SV_POSITION;
	float2  UV : UV0;
    float4  Color    : COLOR0;
};

struct PS_OUTPUT
{
    float4  Color0 : SV_TARGET0;
    float4  Color1 : SV_TARGET1;
};


PS_OUTPUT main(PS_INPUT input)
{ 
	PS_OUTPUT output;

	float4 c;
	c = txt.Sample(smp, input.UV);
	c.a = 255;
	output.Color0 = c;

	c.r = 1.0f - c.r;
	c.g = 1.0f - c.g;
	c.b = 1.0f - c.b;
	output.Color1 = c;

	return output;
}
