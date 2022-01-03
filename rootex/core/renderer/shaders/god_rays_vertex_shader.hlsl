struct GodRaysVSOutput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

struct VSInput
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
};

GodRaysVSOutput main(VSInput input)
{
	GodRaysVSOutput output;
	output.pos = float4(input.pos, 1.0f);
	output.tex = input.tex;

	return output;
}
