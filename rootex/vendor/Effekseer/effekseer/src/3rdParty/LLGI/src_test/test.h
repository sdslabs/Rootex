
#pragma once

#include <LLGI.CommandList.h>
#include <LLGI.Compiler.h>
#include <LLGI.ConstantBuffer.h>
#include <LLGI.Graphics.h>
#include <LLGI.IndexBuffer.h>
#include <LLGI.PipelineState.h>
#include <LLGI.Platform.h>
#include <LLGI.Shader.h>
#include <LLGI.Texture.h>
#include <LLGI.VertexBuffer.h>

#include <iostream>

struct SimpleVertex
{
	LLGI::Vec3F Pos;
	LLGI::Vec2F UV;
	LLGI::Color8 Color;
};

struct SimpleVertexF
{
	LLGI::Vec3F Pos;
	LLGI::Vec2F UV;
	LLGI::ColorF Color;
};
