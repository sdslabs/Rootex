#pragma once

#include "common/common.h"

class Shader
{
protected:
	String m_VertexPath;
	String m_FragmentPath;

public:
	Shader(const String& vertexPath, const String& fragmentPath)
	    : m_VertexPath(vertexPath)
	    , m_FragmentPath(fragmentPath)
	{
	}
	virtual ~Shader() {}

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	// Use for float values
	virtual void setValue(const String& name, const float& value) = 0;
	// Use for Vector3
	virtual void setValue(const String& name, const float& value1, const float& value2, const float& value3) = 0;
	// Use for Vector4
	virtual void setValue(const String& name, const float& value1, const float& value2, const float& value3, const float& value4) = 0;
	// Use for Matrix4x4
	virtual void setValue(const String& name, const Vector<float>& mat) = 0;
};
