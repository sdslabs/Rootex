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
};

class VertexBufferFormat
{
	Vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;

public:
	VertexBufferFormat()
	    : m_Stride(0) {};

	template <typename T>
	void push(int count)
	{
		static_assert(false);
	}

	template <>
	void push<float>(int count)
	{
		m_Elements.push_back({ VertexBufferElement::FLOAT, count, false });
		m_Stride += sizeof(float);
	}
	
	template <>
	void push<unsigned int>(int count)
	{
		m_Elements.push_back({ VertexBufferElement::UNSIGNED_INT, count, false });
		m_Stride += sizeof(unsigned int);
	}

	template <>
	void push<unsigned char>(int count)
	{
		m_Elements.push_back({ VertexBufferElement::BYTE, count, true });
		m_Stride += sizeof(unsigned char);
	}

	const Vector<VertexBufferElement>& getElements() const { return m_Elements; }
	const float& getStride() const { return m_Stride; }
};
