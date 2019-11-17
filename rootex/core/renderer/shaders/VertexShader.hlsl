struct VSOut
{
	float3 color : COLOR;
	float4 pos : SV_POSITION;
};

cbuffer CBuf {
	matrix transform;
};

VSOut main(float2 pos: POSITION, float3 color: COLOR)
{
	VSOut output;
	output.pos = mul(float4(pos.x, pos.y, 0, 1.0f), transform);
	output.color = color;
	return output;
}