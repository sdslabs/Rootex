#pragma once
#include <d3d11.h>

class RootexGraphics
{
private:
	ID3D11Device* pDevice;
	IDXGISwapChain* pSwapChain;
	ID3D11DeviceContext* pContext;
	ID3D11RenderTargetView* pTarget;

public:
	RootexGraphics( HWND windowHandler );
	RootexGraphics(const RootexGraphics&) = delete;
	RootexGraphics& operator=(const RootexGraphics&) = delete;  
	~RootexGraphics();

	void ClearBuffer(float r, float g, float b);
	void EndFrame();
};