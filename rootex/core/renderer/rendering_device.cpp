#include "rendering_device.h"

#include "common/common.h"
#include "dxgiDebugInterface.h"
#include "utils.h"

#include "vendor/DirectXTK/Inc/WICTextureLoader.h"

RenderingDevice::RenderingDevice()
{
}

RenderingDevice::~RenderingDevice()
{
	SafeRelease(&m_Device);
	SafeRelease(&m_Context);
	SafeRelease(&m_SwapChain);
	SafeRelease(&m_RenderTargetView);
	SafeRelease(&m_DepthStencilView);
	CoUninitialize();
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
	sd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_SEQUENTIAL;
	sd.Flags = 0;

	IDXGIDevice* dxgiDevice = 0;
	m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter = 0;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	IDXGIFactory* dxgiFactory = 0;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

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

	//REMARK- reversed winding order to allow ccw .obj files to be rendered properly, can trouble later
	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthBias = 0;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.ScissorEnable = FALSE;
	rsDesc.MultisampleEnable = FALSE;
	rsDesc.AntialiasedLineEnable = FALSE;

	ID3D11RasterizerState* rsState;
	GFX_ERR_CHECK(m_Device->CreateRasterizerState(&rsDesc, &rsState));
	m_Context->RSSetState(rsState);
	SafeRelease(&rsState);

	if (FAILED(CoInitialize(nullptr)))
	{
		ERR("CoInitialize failed");
	}
}

ID3DBlob* RenderingDevice::createBlob(LPCWSTR path)
{
	ID3DBlob* pBlob = nullptr;
	GFX_ERR_CHECK(D3DReadFileToBlob(path, &pBlob));
	return pBlob;
}

void RenderingDevice::enableSkyDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC DSDesc;
	ZeroMemory(&DSDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	DSDesc.DepthEnable = TRUE;
	DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DSDesc.DepthFunc = D3D11_COMPARISON_LESS;
	DSDesc.StencilEnable = FALSE;
	m_Device->CreateDepthStencilState(&DSDesc, &m_NewSkyDepthStencilState);
	//DXUT_SetDebugName(m_pSkyboxDepthStencilState, “SkyboxDepthStencil” );
	m_Context->OMGetDepthStencilState(&m_OldSkyDepthStencilState, &m_StencilRef);
	m_Context->OMSetDepthStencilState(m_NewSkyDepthStencilState, 0);
}

void RenderingDevice::disableSkyDepthStencilState()
{
	m_Context->OMSetDepthStencilState(m_OldSkyDepthStencilState, m_StencilRef);
}

ID3D11Buffer* RenderingDevice::initVertexBuffer(D3D11_BUFFER_DESC* vbd, D3D11_SUBRESOURCE_DATA* vsd, const UINT* stride, const UINT* const offset)
{
	ID3D11Buffer* vertexBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(vbd, vsd, &vertexBuffer));
	return vertexBuffer;
}

ID3D11Buffer* RenderingDevice::initIndexBuffer(D3D11_BUFFER_DESC* ibd, D3D11_SUBRESOURCE_DATA* isd, DXGI_FORMAT format)
{
	ID3D11Buffer* indexBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(ibd, isd, &indexBuffer));
	return indexBuffer;
}

void RenderingDevice::initVSModelConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd)
{
	ID3D11Buffer* pConstantBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(cbd, csd, &pConstantBuffer));
	m_Context->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);

	SafeRelease(&pConstantBuffer);
}

void RenderingDevice::initVSViewConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd)
{
	ID3D11Buffer* pConstantBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(cbd, csd, &pConstantBuffer));
	m_Context->VSSetConstantBuffers(1u, 1u, &pConstantBuffer);

	SafeRelease(&pConstantBuffer);
}

void RenderingDevice::initVSProjectionConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd)
{
	ID3D11Buffer* pConstantBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(cbd, csd, &pConstantBuffer));
	m_Context->VSSetConstantBuffers(2u, 1u, &pConstantBuffer);

	SafeRelease(&pConstantBuffer);
}

void RenderingDevice::initPSConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd, UINT offset)
{
	ID3D11Buffer* pConstantBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(cbd, csd, &pConstantBuffer));
	m_Context->PSSetConstantBuffers(offset, 1u, &pConstantBuffer);

	SafeRelease(&pConstantBuffer);
}

ID3D11PixelShader* RenderingDevice::initPixelShader(ID3DBlob* blob)
{
	ID3D11PixelShader* pixelShader = nullptr;
	GFX_ERR_CHECK(m_Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader));
	return pixelShader;
}

ID3D11VertexShader* RenderingDevice::initVertexShader(ID3DBlob* blob)
{
	ID3D11VertexShader* vertexShader = nullptr;
	GFX_ERR_CHECK(m_Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader));
	return vertexShader;
}

void RenderingDevice::initVertexLayout(ID3DBlob* vertexShaderBlob, const D3D11_INPUT_ELEMENT_DESC* ied, UINT size)
{
	ID3D11InputLayout* inputLayout = nullptr;
	GFX_ERR_CHECK(m_Device->CreateInputLayout(
	    ied, size,
	    vertexShaderBlob->GetBufferPointer(),
	    vertexShaderBlob->GetBufferSize(),
	    &inputLayout));

	SafeRelease(&vertexShaderBlob);

	//bind vertex layout
	m_Context->IASetInputLayout(inputLayout);

	SafeRelease(&inputLayout);
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderingDevice::createTexture(ImageResourceFile* imageRes)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> textureResource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;

	if (FAILED(DirectX::CreateWICTextureFromMemory(m_Device, (const uint8_t*)imageRes->getData()->getRawData()->data(), (size_t)imageRes->getData()->getRawDataByteSize(), textureResource.GetAddressOf(), textureView.GetAddressOf())))
	{
		ERR("Could not create texture: " + imageRes->getPath().generic_string());
	}

	return textureView;
}

void RenderingDevice::bind(ID3D11Buffer* vertexBuffer, const unsigned int* stride, const unsigned int* offset)
{
	m_Context->IASetVertexBuffers(0u, 1u, &vertexBuffer, stride, offset);
}

void RenderingDevice::bind(ID3D11Buffer* indexBuffer, DXGI_FORMAT format)
{
	m_Context->IASetIndexBuffer(indexBuffer, format, 0u);
}

void RenderingDevice::bind(ID3D11VertexShader* vertexShader)
{
	m_Context->VSSetShader(vertexShader, nullptr, 0u);
}

void RenderingDevice::bind(ID3D11PixelShader* pixelShader)
{
	m_Context->PSSetShader(pixelShader, nullptr, 0u);
}

void RenderingDevice::setInPixelShader(unsigned int slot, unsigned int number, ID3D11ShaderResourceView* texture)
{
	m_Context->PSSetShaderResources(slot, number, &texture);
}

void RenderingDevice::setInPixelShader(ID3D11SamplerState* samplerState)
{
	m_Context->PSSetSamplers(0, 1, &samplerState);
}

void RenderingDevice::unbindShaderResources()
{
	m_Context->VSSetShaderResources(0, 1, nullptr);
	m_Context->PSSetShaderResources(0, 1, nullptr);
}

void RenderingDevice::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY pt)
{
	m_Context->IASetPrimitiveTopology(pt);
}

void RenderingDevice::setViewport(const D3D11_VIEWPORT* vp)
{
	m_Context->RSSetViewports(1u, vp);
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> RenderingDevice::createSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState; 
	if (FAILED(m_Device->CreateSamplerState(&samplerDesc, &samplerState)))
	{
		ERR("SamplerState could not be created");
	}

	return samplerState;
}

void RenderingDevice::drawIndexed(UINT number)
{
	m_Context->DrawIndexed(number, 0u, 0u);
}

RenderingDevice* RenderingDevice::GetSingleton()
{
	static RenderingDevice singleton;
	return &singleton;
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
