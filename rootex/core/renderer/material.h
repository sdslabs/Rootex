#pragma once

#include "shader.h"
#include "constant_buffer.h"

struct Material
{
	VSConstantBuffer m_VSConstantBuffer;
	PSConstantBuffer m_PSConstantBuffer;

	Material() = default;
	Material(const VSConstantBuffer& vcb, const PSConstantBuffer& pcb)
	    : m_VSConstantBuffer(vcb)
	    , m_PSConstantBuffer(pcb)
	{
	}
};
