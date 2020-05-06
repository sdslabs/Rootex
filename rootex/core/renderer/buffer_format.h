#pragma once

#include <dxgi.h>

#include "common/common.h"

/// Struct encapsulating the vertex buffer formats renderer currently supports
struct VertexBufferElement
{
	/// Abstracts the DXGI input format types
	enum Type
	{
		POSITION = DXGI_FORMAT_R32G32B32_FLOAT,
		TEXCOORD = DXGI_FORMAT_R32G32_FLOAT,
		BONEWEIGHTS = DXGI_FORMAT_R32G32B32A32_FLOAT,
		BONEINDICES = DXGI_FORMAT_R16G16B16A16_UINT
	};

	/// What type of objects are present in buffer
	Type m_Type; 
	/// Used as the semantic of the Vertex Buffer element in shaders
	LPCSTR m_Name;

	/// Total size of the Vertex Buffer
	static unsigned int GetSize(Type type)
	{
		switch (type)
		{
		case POSITION:
			return sizeof(float) * 3;
		case TEXCOORD:
			return sizeof(float) * 2;
		case BONEWEIGHTS:
			return sizeof(float) * 4;
		case BONEINDICES:
			return sizeof(unsigned int) * 4;
		default:
			ERR("Unknown size found");
			return 0;
		}
	}
};

/// Stores vector of Vertex Buffer Elements to be used as the input for Vertex Shaders
class BufferFormat
{
	Vector<VertexBufferElement> m_Elements;
	
public:
	BufferFormat() = default;

	void push(VertexBufferElement::Type type, LPCSTR name) { m_Elements.push_back({ type, name }); }

	const Vector<VertexBufferElement>& getElements() const { return m_Elements; }
};
