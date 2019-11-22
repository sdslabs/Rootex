cbuffer CBuf 
{
	float4 face_colors[6];
};

float4 main(uint tid : SV_PRIMITIVEID) : SV_TARGET
{
	return face_colors[tid / 2];
}