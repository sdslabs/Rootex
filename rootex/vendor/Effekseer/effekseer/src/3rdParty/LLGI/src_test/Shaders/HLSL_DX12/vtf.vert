struct VS_INPUT{
    float3 g_position : POSITION0;
    float2 g_uv : UV0;
    float4 g_color : COLOR0;
};
struct VS_OUTPUT{
    float4 g_position : SV_POSITION;
    float4 g_color : COLOR0;
};

Texture2D txt : register(t0);
SamplerState smp : register(s0);

VS_OUTPUT main(VS_INPUT input){
    VS_OUTPUT output;
    float4 c = txt.SampleLevel(smp, input.g_uv, 0);

    output.g_position = float4(input.g_position, 1.0f);
    output.g_position.xy += c.xy;
    output.g_color = input.g_color;
    
    return output;
}
