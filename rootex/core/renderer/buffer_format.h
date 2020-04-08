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
		TEXCOORD = DXGI_FORMAT_R32G32_FLOAT
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
			return sizeof(float) + sizeof(float) + sizeof(float);
		case TEXCOORD:
			return sizeof(float) + sizeof(float);
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
