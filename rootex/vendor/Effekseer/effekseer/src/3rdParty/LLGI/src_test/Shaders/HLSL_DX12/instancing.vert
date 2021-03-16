struct VS_INPUT{
    float3 g_position : POSITION0;
    float2 g_uv : UV0;
    float4 g_color : COLOR0;
    uint InstanceId : SV_InstanceID;
};
struct VS_OUTPUT{
    float4 g_position : SV_POSITION;
    float4 g_color : COLOR0;
};

cbuffer CB : register(b0)
{
  float4 offsets[10];
};

VS_OUTPUT main(VS_INPUT input){
    VS_OUTPUT output;
    
    output.g_position = float4(input.g_position, 1.0f);
    output.g_position.x += offsets[input.InstanceId].x;
    output.g_position.y += offsets[input.InstanceId].y;
    output.g_color = input.g_color;
    
    return output;
}
