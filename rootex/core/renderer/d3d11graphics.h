#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <array>
#include <iostream>
#include <DirectXMath.h>
#include "d3d11utils.h"
#include "dxgiDebugInterface.h"

#define GFX_ERR_CHECK(hr)      \
	dxgiDebugInterface.Set();  \
	if (FAILED(hr)) \
	dxgiDebugInterface.GetMessages(__FILE__, __func__)

class RootexGraphics
{
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
	ID3D11DepthStencilView* pDSView = nullptr;

	unsigned int height;
	unsigned int width;
	float maxX = 1.0f;
	float minZ = 0.5f;
	float maxZ = 10.0f;

public:
	RootexGraphics( HWND windowHandler, unsigned int w = 640, unsigned int h = 480 );
	RootexGraphics(const RootexGraphics&) = delete;
	RootexGraphics& operator=(const RootexGraphics&) = delete;  
	~RootexGraphics();

	DxgiDebugInterface dxgiDebugInterface;

	void ClearBuffer(float r, float g, float b);
	void DrawTestCube(float angle);
	void EndFrame();
};