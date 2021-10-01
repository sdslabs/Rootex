struct CustomPostProcessingVSOutput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

CustomPostProcessingVSOutput main(uint vI : SV_VertexId)
{
	CustomPostProcessingVSOutput output;

	float2 texcoord = float2((vI << 1) & 2, vI & 2);
	output.tex = texcoord;

	output.pos = float4(texcoord.x * 2 - 1, -texcoord.y * 2 + 1, 0, 1);
	return output;
}
