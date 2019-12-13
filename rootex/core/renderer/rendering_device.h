#pragma once

#include <d3d11.h>

class RenderingDevice
{
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_Context;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;

	IDXGISwapChain* m_SwapChain;
	unsigned int m_4XMSQuality;

	RenderingDevice();
	RenderingDevice(RenderingDevice&) = delete;
	~RenderingDevice();

	// Should only be called by Window class
	void swapBuffers();

	friend class Window;

public:
	static RenderingDevice* GetSingleton();

	void initialize(HWND hWnd, int width, int height);

	void clearBuffer(float r, float g, float b);
};
