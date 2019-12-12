#pragma once

#include "renderer/renderer.h"

class OpenGLRenderer : public Renderer
{
	OpenGLRenderer() {}
	virtual ~OpenGLRenderer() {}

public:
	void clear() const override;
	void draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader) override;
};
