struct PS_INPUT{
    float4 g_position : SV_POSITION;
    float4 g_color : COLOR0;
};

float4 main(PS_INPUT input) : SV_TARGET{
    return input.g_color;
}
