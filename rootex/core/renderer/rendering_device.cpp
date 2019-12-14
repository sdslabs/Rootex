#include "rendering_device.h"

#include "common/common.h"
#include "dxgiDebugInterface.h"
#include "d3d11utils.h"

RenderingDevice::RenderingDevice()
{
}

RenderingDevice::~RenderingDevice()
{
	SafeRelease(&m_Device);
	SafeRelease(&m_SwapChain);
	SafeRelease(&m_Context);
}

void RenderingDevice::initialize(HWND hWnd, int width, int height)
{
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel = {};

	HRESULT hr = D3D11CreateDevice(0, // Default adapter
	    D3D_DRIVER_TYPE_HARDWARE,
	    0, // No software device
	    createDeviceFlags,
	    0,
	    0, // Default feature level array
	    D3D11_SDK_VERSION,
	    &m_Device,
	    &featureLevel,
	    &m_Context);

	if (FAILED(hr))
	{
		ERR("D3D11CreateDevice Failed.");
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		ERR("Direct3D Feature Level 11 unsupported.");
	}

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	m_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4XMSQuality);
	PANIC(m_4XMSQuality <= 0, "MSAA is not supported on this hardware");

	//if (m_4XMSQuality)
	if (false)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_4XMSQuality - 1;
	}
	else // No MSAA
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	IDXGIDevice* dxgiDevice = 0;
	m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	
	IDXGIAdapter* dxgiAdapter = 0; 
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	IDXGIFactory* dxgiFactory = 0; 
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory),(void**)&dxgiFactory);

	dxgiFactory->CreateSwapChain(m_Device, &sd, &m_SwapChain);
	
	SafeRelease(&dxgiDevice);
	SafeRelease(&dxgiAdapter);
	SafeRelease(&dxgiFactory);

	ID3D11Resource* backBuffer = nullptr;
	GFX_ERR_CHECK(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&backBuffer)));
	GFX_ERR_CHECK(m_Device->CreateRenderTargetView(
	    backBuffer,
	    nullptr,
	    &m_RenderTargetView));
	SafeRelease(&backBuffer);

	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	ID3D11DepthStencilState* DSState;
	GFX_ERR_CHECK(m_Device->CreateDepthStencilState(&dsDesc, &DSState));
	m_Context->OMSetDepthStencilState(DSState, 1u);
	SafeRelease(&DSState);

	ID3D11Texture2D* depthStencil = nullptr;
	D3D11_TEXTURE2D_DESC descDepth = { 0 };
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_ERR_CHECK(m_Device->CreateTexture2D(&descDepth, nullptr, &depthStencil));
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSView = {};
	descDSView.Format = DXGI_FORMAT_D32_FLOAT;
	descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSView.Texture2D.MipSlice = 0u;
	GFX_ERR_CHECK(m_Device->CreateDepthStencilView(depthStencil, &descDSView, &m_DepthStencilView));
	SafeRelease(&depthStencil);

	m_Context->OMSetRenderTargets(1u, &m_RenderTargetView, m_DepthStencilView);
}

RenderingDevice* RenderingDevice::GetSingleton()
{
	static RenderingDevice singleton;
	return &singleton;
}

ID3D11Device* RenderingDevice::GetDevice()
{
	return m_Device;
}

ID3D11DeviceContext* RenderingDevice::GetContext()
{
	return m_Context;
}

void RenderingDevice::swapBuffers()
{
	GFX_ERR_CHECK(m_SwapChain->Present(1u, 0));
}

void RenderingDevice::clearBuffer(float r, float g, float b)
{
	const float color[] = { r, g, b, 1.0f };
	m_Context->ClearRenderTargetView(m_RenderTargetView, color);
	m_Context->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u);
}
