struct VS_INPUT{
    float3 Position : POSITION0;
	float2 UV : UV0;
    float4 Color : COLOR0;
};
struct VS_OUTPUT{
    float4 Position : SV_POSITION;
	float2 UV : UV0;
    float4 Color : COLOR0;
};
   

cbuffer CB : register(b0)
{
  float4 offset;
};

VS_OUTPUT main(VS_INPUT input){
    VS_OUTPUT output;
        
    output.Position = float4(input.Position, 1.0f) + offset;
	output.UV = input.UV;
    output.Color = input.Color;
        
    return output;
}
