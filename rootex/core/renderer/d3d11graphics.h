#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <array>
#include <iostream>
#include <DirectXMath.h>

class RootexGraphics
{
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;

public:
	RootexGraphics( HWND windowHandler );
	RootexGraphics(const RootexGraphics&) = delete;
	RootexGraphics& operator=(const RootexGraphics&) = delete;  
	~RootexGraphics();

	void ClearBuffer(float r, float g, float b);
	void DrawTestCube(float angle);
	void EndFrame();
};
template <class T>
void SafeRelease(T** ppT);