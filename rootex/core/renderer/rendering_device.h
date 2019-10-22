#pragma once

#include <d3d11.h>

#include "common/types.h"

class IRenderingDevice
{
public:
	virtual void endFrame() = 0;
};

class RenderingDeviceD3D : public IRenderingDevice
{
private:
	Ptr<ID3D11Device> m_Device;
	Ptr<IDXGISwapChain> m_SwapChain;
	Ptr<ID3D11DeviceContext> m_Context;

public:
	RenderingDeviceD3D(HWND windowHandler);
	RenderingDeviceD3D(const RenderingDeviceD3D&) = delete;
	RenderingDeviceD3D& operator=(const RenderingDeviceD3D&) = delete;
	~RenderingDeviceD3D();

	void endFrame() override;
};