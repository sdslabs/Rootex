#pragma once

#include <dxgi.h>

#include "common/common.h"

struct VertexBufferElement
{
	enum Type
	{
		POSITION = DXGI_FORMAT_R32G32B32_FLOAT,
		TEXCOORD = DXGI_FORMAT_R32G32_FLOAT
	};

	Type m_Type; // What type of objects are present in buffer
	LPCSTR m_Name;

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

class BufferFormat
{
	Vector<VertexBufferElement> m_Elements;
	
public:
	BufferFormat() = default;

	void push(VertexBufferElement::Type type, LPCSTR name) { m_Elements.push_back({ type, name }); }

	const Vector<VertexBufferElement>& getElements() const { return m_Elements; }
};
