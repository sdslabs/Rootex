#include "rendering_device.h"

#include "common/common.h"
#include "dxgi_debug_interface.h"

#include "vendor/DirectXTK/Inc/WICTextureLoader.h"

RenderingDevice::RenderingDevice()
{
	GFX_ERR_CHECK(CoInitialize(nullptr));
}

RenderingDevice::~RenderingDevice()
{
	m_SwapChain->SetFullscreenState(false, nullptr);
	CoUninitialize();
}

void RenderingDevice::setScreenState(bool fullscreen)
{
	m_SwapChain->SetFullscreenState(fullscreen, nullptr);
}

void RenderingDevice::initialize(HWND hWnd, int width, int height, bool MSAA)
{
	m_MSAA = MSAA;
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

	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	m_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4XMSQuality);
	PANIC(m_4XMSQuality <= 0, "MSAA is not supported on this hardware");

	if (m_4XMSQuality && MSAA)
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

	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice = 0;
	m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter = 0;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory = 0;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	dxgiFactory->CreateSwapChain(m_Device.Get(), &sd, &m_SwapChain);

	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	D3D11_FEATURE_DATA_D3D11_OPTIONS features;
	GFX_ERR_CHECK(m_Device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS, &features, sizeof(features)));

	PRINT(
		"Supported DirectX11 Features\n"
		"MapNoOverwriteOnDynamicConstantBuffer: " + std::to_string(features.MapNoOverwriteOnDynamicConstantBuffer));

	GFX_ERR_CHECK(m_Device->CreateDepthStencilState(&dsDesc, &m_DepthStencilState));
	m_Context->OMSetDepthStencilState(m_DepthStencilState.Get(), 1u);
	m_StencilRef = 1u;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil = nullptr;
	D3D11_TEXTURE2D_DESC descDepth = { 0 };
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = sd.SampleDesc.Count;
	descDepth.SampleDesc.Quality = sd.SampleDesc.Quality;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_ERR_CHECK(m_Device->CreateTexture2D(&descDepth, nullptr, &depthStencil));
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSView = {};
	descDSView.Format = DXGI_FORMAT_D32_FLOAT;
	descDSView.ViewDimension = MSAA ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSView.Texture2D.MipSlice = 0u;
	GFX_ERR_CHECK(m_Device->CreateDepthStencilView(depthStencil.Get(), &descDSView, &m_DepthStencilView));

	m_CurrentRenderTarget = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Resource> backBuffer = nullptr;
	GFX_ERR_CHECK(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf())));
	GFX_ERR_CHECK(m_Device->CreateRenderTargetView(
	    backBuffer.Get(),
	    nullptr,
	    &m_RenderTargetBackBufferView));

	createRenderTextureTarget(width, height);

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
	rsDesc.MultisampleEnable = MSAA;
	rsDesc.AntialiasedLineEnable = FALSE;

	GFX_ERR_CHECK(m_Device->CreateRasterizerState(&rsDesc, &m_RSState));
	m_Context->RSSetState(m_RSState.Get());

	setTextureRenderTarget();

	m_FontBatch.reset(new DirectX::SpriteBatch(m_Context.Get()));
}

Ref<DirectX::SpriteFont> RenderingDevice::createFont(FileBuffer* fontFileBuffer)
{
	return Ref<DirectX::SpriteFont>(new DirectX::SpriteFont(m_Device.Get(), (const uint8_t*)fontFileBuffer->data(), fontFileBuffer->size()));
}

Microsoft::WRL::ComPtr<ID3DBlob> RenderingDevice::createBlob(LPCWSTR path)
{
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob = nullptr;
	GFX_ERR_CHECK(D3DReadFileToBlob(path, &pBlob));
	return pBlob;
}

void RenderingDevice::createRenderTextureTarget(int width, int height)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = m_MSAA ? 4 : 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	GFX_ERR_CHECK(m_Device->CreateTexture2D(&textureDesc, NULL, &m_RenderTargetTexture));

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = m_MSAA ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	GFX_ERR_CHECK(m_Device->CreateRenderTargetView(m_RenderTargetTexture.Get(), &renderTargetViewDesc, &m_RenderTargetTextureView));

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = m_MSAA ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	GFX_ERR_CHECK(m_Device->CreateShaderResourceView(m_RenderTargetTexture.Get(), &shaderResourceViewDesc, &m_RenderTextureShaderResourceView));
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
	m_Context->OMSetDepthStencilState(m_NewSkyDepthStencilState.Get(), 0);
}

void RenderingDevice::disableSkyDepthStencilState()
{
	m_Context->OMSetDepthStencilState(m_OldSkyDepthStencilState.Get(), m_StencilRef);
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderingDevice::createVertexBuffer(D3D11_BUFFER_DESC* vbd, D3D11_SUBRESOURCE_DATA* vsd, const UINT* stride, const UINT* const offset)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(vbd, vsd, &vertexBuffer));
	return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderingDevice::createIndexBuffer(D3D11_BUFFER_DESC* ibd, D3D11_SUBRESOURCE_DATA* isd, DXGI_FORMAT format)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(ibd, isd, &indexBuffer));
	return indexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderingDevice::createVSProjectionConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(cbd, csd, &constantBuffer));
	m_Context->VSSetConstantBuffers(3u, 1u, constantBuffer.GetAddressOf());
	return constantBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderingDevice::createVSPerFrameConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(cbd, csd, &constantBuffer));
	m_Context->VSSetConstantBuffers(2u, 1u, constantBuffer.GetAddressOf());
	return constantBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderingDevice::createPSPerFrameConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(cbd, csd, &constantBuffer));
	m_Context->PSSetConstantBuffers(2u, 1u, constantBuffer.GetAddressOf());
	return constantBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderingDevice::createVSConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd, UINT slot)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(cbd, csd, &constantBuffer));
	m_Context->VSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf());
	return constantBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderingDevice::createPSConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd, UINT slot)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(cbd, csd, &constantBuffer));
	m_Context->PSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf());
	return constantBuffer;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> RenderingDevice::createPixelShader(ID3DBlob* blob)
{
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader = nullptr;
	GFX_ERR_CHECK(m_Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader));
	return pixelShader;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> RenderingDevice::createVertexShader(ID3DBlob* blob)
{
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader = nullptr;
	GFX_ERR_CHECK(m_Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader));
	return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11InputLayout> RenderingDevice::createVertexLayout(ID3DBlob* vertexShaderBlob, const D3D11_INPUT_ELEMENT_DESC* ied, UINT size)
{
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	GFX_ERR_CHECK(m_Device->CreateInputLayout(
	    ied, size,
	    vertexShaderBlob->GetBufferPointer(),
	    vertexShaderBlob->GetBufferSize(),
	    &inputLayout));

	m_Context->IASetInputLayout(inputLayout.Get());

	return inputLayout;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderingDevice::createTexture(ImageResourceFile* imageRes)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> textureResource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;

	if (FAILED(DirectX::CreateWICTextureFromMemory(m_Device.Get(), (const uint8_t*)imageRes->getData()->getRawData()->data(), (size_t)imageRes->getData()->getRawDataByteSize(), textureResource.GetAddressOf(), textureView.GetAddressOf())))
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

void RenderingDevice::bind(ID3D11InputLayout* inputLayout)
{
	m_Context->IASetInputLayout(inputLayout);
}

//Assuming subresource offset = 0
void RenderingDevice::mapBuffer(ID3D11Buffer* buffer, D3D11_MAPPED_SUBRESOURCE& subresource)
{
	if (FAILED(m_Context->Map(buffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &subresource)))
	{
		ERR("Could not map to constant buffer");
	}
}

//Assuming subresource offset = 0
void RenderingDevice::unmapBuffer(ID3D11Buffer* buffer)
{
	m_Context->Unmap(buffer, 0);
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

void RenderingDevice::setRasterizerState()
{
	m_Context->RSSetState(m_RSState.Get());
}

void RenderingDevice::setDepthStencilState()
{
	m_Context->OMSetDepthStencilState(m_DepthStencilState.Get(), m_StencilRef);
}

void RenderingDevice::setTextureRenderTarget()
{
	m_Context->OMSetRenderTargets(1, m_RenderTargetTextureView.GetAddressOf(), m_DepthStencilView.Get());
	m_CurrentRenderTarget = m_RenderTargetTextureView.GetAddressOf();
	m_UnboundRenderTarget = m_RenderTargetBackBufferView.GetAddressOf();
}

void RenderingDevice::setBackBufferRenderTarget()
{
	m_Context->OMSetRenderTargets(1, m_RenderTargetBackBufferView.GetAddressOf(), m_DepthStencilView.Get());
	m_CurrentRenderTarget = m_RenderTargetBackBufferView.GetAddressOf();
	m_UnboundRenderTarget = m_RenderTargetTextureView.GetAddressOf();
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderingDevice::getRenderTextureShaderResourceView()
{
	return m_RenderTextureShaderResourceView;
}

Ref<DirectX::SpriteBatch> RenderingDevice::getUIBatch()
{
	return m_FontBatch;
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

void RenderingDevice::beginDrawUI()
{
	m_FontBatch->Begin();
}

void RenderingDevice::endDrawUI()
{
	m_FontBatch->End();
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

void RenderingDevice::clearCurrentRenderTarget(float r, float g, float b)
{
	const float color[] = { r, g, b, 1.0f };
	m_Context->ClearRenderTargetView(*m_CurrentRenderTarget, color);
	m_Context->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void RenderingDevice::clearUnboundRenderTarget(float r, float g, float b)
{
	const float color[] = { r, g, b, 1.0f };
	m_Context->ClearRenderTargetView(*m_UnboundRenderTarget, color);
	m_Context->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

#ifdef ROOTEX_EDITOR
ID3D11Device* RenderingDevice::getDevice()
{
	return m_Device.Get();
}

ID3D11DeviceContext* RenderingDevice::getContext()
{
	return m_Context.Get();
}
#endif // ROOTEX_EDITOR
