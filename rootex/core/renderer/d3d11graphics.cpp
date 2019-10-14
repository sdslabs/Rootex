#include <core/renderer/d3d11graphics.h>

#pragma comment(lib, "d3d11.lib")

RootexGraphics::RootexGraphics(HWND windowHandler)
{
	DXGI_SWAP_CHAIN_DESC descriptor = { 0 };
	descriptor.BufferDesc.Width = 0;
	descriptor.BufferDesc.Height = 0;
	descriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	descriptor.BufferDesc.RefreshRate.Numerator = 0;
	descriptor.BufferDesc.RefreshRate.Denominator = 0;
	descriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	descriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	descriptor.SampleDesc.Count = 1;
	descriptor.SampleDesc.Quality = 0;
	descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descriptor.BufferCount = 1;
	descriptor.OutputWindow = windowHandler;
	descriptor.Windowed = TRUE;
	descriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	descriptor.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
	    nullptr,
	    D3D_DRIVER_TYPE_HARDWARE,
	    nullptr,
	    0,
	    nullptr,
	    0,
	    D3D11_SDK_VERSION,
	    &descriptor,
	    &m_swapChain,
	    &m_device,
	    nullptr,
	    &m_context
	);
}

RootexGraphics::~RootexGraphics()
{
	if (m_context != nullptr)
		m_context->Release();
	if (m_swapChain != nullptr)
		m_swapChain->Release();
	if (m_device != nullptr)
		m_device->Release();
}

void RootexGraphics::EndFrame()
{
	m_swapChain->Present(1u, 0);
}
