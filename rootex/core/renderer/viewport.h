#pragma once

#include <d3d11.h>

/// Encapsulation of the viewport being rendered to
class Viewport
{
	D3D11_VIEWPORT m_Viewport;

public:
	Viewport(float width, float height, float minDepth, float maxDepth, float topLeftX, float topLeftY);
	Viewport(Viewport&) = delete;
	~Viewport();

	const D3D11_VIEWPORT* getViewport() const { return &m_Viewport; }
};
