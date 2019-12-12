#pragma once

#include "common/common.h"

struct VertexBufferElement
{
	enum Type
	{
		BYTE = 0,
		UNSIGNED_INT,
		FLOAT
	};

	Type m_Type; // What type of objects are present in buffer
	unsigned int m_Count; // How many objects of <m_Type> are present in buffer
	unsigned char m_IsNormalized; // Are the values already normalized? (E.g. OpenGL wants positions normalized as [-1.0f, +1.0f])

	static unsigned int GetSize(Type type)
	{
		switch (type)
		{
		case BYTE:
			return sizeof(unsigned char);
			break;
		case UNSIGNED_INT:
			return sizeof(unsigned int);
		case FLOAT:
			return sizeof(float);
		default:
			ERR("Unknown size found");
			return 0;
		}
	}
};

class VertexBufferFormat
{
	Vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;

public:
	VertexBufferFormat()
	    : m_Stride(0) {};

	template <typename T>
	void push(unsigned int count)
	{
		static_assert(false);
	}

	template <>
	void push<float>(unsigned int count)
	{
		m_Elements.push_back({ VertexBufferElement::FLOAT, count, false });
		m_Stride += sizeof(float) * count;
	}

	template <>
	void push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ VertexBufferElement::UNSIGNED_INT, count, false });
		m_Stride += sizeof(unsigned int) * count;
	}

	template <>
	void push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ VertexBufferElement::BYTE, count, true });
		m_Stride += sizeof(unsigned char) * count;
	}

	const Vector<VertexBufferElement>& getElements() const { return m_Elements; }
	const float& getStride() const { return m_Stride; }
};
