#pragma once

#include <dxgi.h>

#include "common/common.h"

/// Struct encapsulating the vertex buffer formats renderer currently supports
struct VertexBufferElement
{
	/// Abstracts the DXGI input format types
	enum Type
	{
		FloatFloatFloatFloat = DXGI_FORMAT_R32G32B32A32_FLOAT,
		FloatFloatFloat = DXGI_FORMAT_R32G32B32_FLOAT,
		FloatFloat = DXGI_FORMAT_R32G32_FLOAT,
		ByteByteByteByte = DXGI_FORMAT_R8G8B8A8_UNORM,
		UInt = DXGI_FORMAT_R32_UINT
		UintUintUintUint = DXGI_FORMAT_R32G32B32A32_FLOAT,
		FloatFloatFloatFloat = DXGI_FORMAT_R32G32B32A32_UINT 
	};

	/// What type of objects are present in buffer
	Type m_Type; 
	/// Used as the semantic of the Vertex Buffer element in shaders
	LPCSTR m_Name;
	/// Per-what kind of object? (Changes for instance buffers mainly)
	D3D11_INPUT_CLASSIFICATION m_Class;
	/// Buffer slot to add this element to
	int m_Slot;
	/// Reset the offset counter. Used to starting a new buffer
	bool m_ResetOffset;
	/// Number of instances to be rendered per instance
	UINT m_RendersPerInstance;

	/// Total size of the Vertex Buffer
	static unsigned int GetSize(Type type)
	{
		switch (type)
		{
		case FloatFloatFloatFloat:
			return sizeof(float) * 4;
		case FloatFloatFloat:
			return sizeof(float) * 3;
		case FloatFloat:
			return sizeof(float) * 2;
		case ByteByteByteByte:
			return sizeof(char) * 4;
		case UInt:
			return sizeof(unsigned int);
		case UintUintUintUint:
			return sizeof(UINT) * 4;
		case FloatFloatFloatFloat:
			return sizeof(float) * 4;
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

	void push(VertexBufferElement::Type type, LPCSTR name, D3D11_INPUT_CLASSIFICATION elementClass, int slot, bool resetOffset, UINT rendersPerInstance) { m_Elements.push_back(VertexBufferElement { type, name, elementClass, slot, resetOffset, rendersPerInstance }); }

	const Vector<VertexBufferElement>& getElements() const { return m_Elements; }
};
