#pragma once

enum class RendererType
{
	DIRECTX,
	OPENGL
};

class Renderer
{
	Renderer* m_Instance;

protected:
	Renderer(RendererType type);
	~Renderer();

public:
};
