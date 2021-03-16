
#include "TestHelper.h"
#include "test.h"

void test_compile(LLGI::DeviceType deviceType)
{
	auto compiler = LLGI::CreateCompiler(deviceType);

	if (compiler == nullptr)
	{
		return;
	}

	LLGI::CompilerResult result_vs;
	LLGI::CompilerResult result_ps;

	auto code_hlsl_vs = R"(

struct VS_Input
{
	float3 Pos		: Pos;
	float2 UV		: UV;
	float4 Color		: Color;
};

struct VS_Output
{
	float4 Pos		: SV_POSITION;
	float2 UV		: TEXCOORD0;
	float4 Color		: COLOR;
};

VS_Output main( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };
	Output.Pos = pos4;
	Output.Color = Input.Color;
	Output.UV = Input.UV;
	return Output;
}

)";

	auto code_hlsl_ps = R"(

struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float2 UV		: TEXCOORD0;
	float4 Color		: COLOR;
};


float4 main( const PS_Input Input ) : SV_Target
{
	float4 Output = Input.Color;

	if(Output.a == 0.0f) discard;

	return Output;
}

)";

	auto code_glsl_vs = R"(
#version 440 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uv;

out gl_PerVertex
{
    vec4 gl_Position;
};

out vec2 v_uv;

void main()
{
	gl_Position.x  = a_position.x;
	gl_Position.y  = a_position.y;
	gl_Position.z  = a_position.z;
	gl_Position.w  = 1.0f;
	v_uv  = a_uv;
}

)";

	auto code_glsl_ps = R"(
#version 440 core
#extension GL_NV_gpu_shader5:require

in vec2 v_uv;

layout(location = 0) out vec4 color;

void main()
{
   color  = vec4(1.0, 1.0, 1.0, 1.0);
}

)";

	auto code_metal_vs = R"(

    struct VertexIn {
        metal::packed_float3 position;
        metal::packed_float2 uv;
    };
    
    struct VertexOut {
        metal::float4 position [[position]];
        metal::float2 uv;
    };
    
    vertex VertexOut vertex_shader_function(const device VertexIn *vertex_array [[buffer(0)]], unsigned int vid [[vertex_id]]) {
        
        VertexOut vo;
        vo.position = metal::float4(vertex_array[vid].position, 1.0);
        vo.uv = vertex_array[vid].uv;
        return vo;
    }
    
)";

	auto code_metal_ps = R"(

    
    struct VertexOut {
        metal::float4 position [[position]];
        metal::float2 uv;
    };
    
    fragment metal::half4 basic_fragment(VertexOut input [[stage_in]]) {
        return metal::half4(1.0);
    }
    
    )";

	if (compiler->GetDeviceType() == LLGI::DeviceType::DirectX12)
	{
		compiler->Compile(result_vs, code_hlsl_vs, LLGI::ShaderStageType::Vertex);
		compiler->Compile(result_ps, code_hlsl_ps, LLGI::ShaderStageType::Pixel);
	}
	else if (compiler->GetDeviceType() == LLGI::DeviceType::Metal)
	{
		compiler->Compile(result_vs, code_metal_vs, LLGI::ShaderStageType::Vertex);
		compiler->Compile(result_ps, code_metal_ps, LLGI::ShaderStageType::Pixel);
	}
	else
	{
		compiler->Compile(result_vs, code_glsl_vs, LLGI::ShaderStageType::Vertex);
		compiler->Compile(result_ps, code_glsl_ps, LLGI::ShaderStageType::Pixel);
	}

	std::cout << result_vs.Message.c_str() << std::endl;
	std::cout << result_ps.Message.c_str() << std::endl;

	LLGI::SafeRelease(compiler);
}

TestRegister Compile_Basic("Compile.Basic", [](LLGI::DeviceType device) -> void { test_compile(device); });
