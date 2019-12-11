#include "renderer.h"

#include "directx/directx_renderer.h"
#include "opengl/opengl_renderer.h"

Renderer::Renderer(RendererType type)
{
	switch (type)
	{
	case RendererType::DIRECTX:
		//m_Instance = new DirectXRenderer();
		break;
	case RendererType::OPENGL:
		//m_Instance = new OpenGLRenderer();
		break;
	default:
		break;
	}
}

Renderer::~Renderer()
{
}
