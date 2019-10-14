#pragma once
#include <d3d11.h>

class RootexGraphics
{
private:
	ID3D11Device* m_device;
	IDXGISwapChain* m_swapChain;
	ID3D11DeviceContext* m_context;

public:
	RootexGraphics( HWND windowHandler );
	RootexGraphics(const RootexGraphics&) = delete;
	RootexGraphics& operator=(const RootexGraphics&) = delete;  
	~RootexGraphics();

	void EndFrame();
};