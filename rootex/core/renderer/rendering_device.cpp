#include "rendering_device.h"

#include <locale>
#include <codecvt>

#include "common/common.h"
#include "core/event_manager.h"
#include "dxgi_debug_interface.h"

#include "vendor/DirectXTK/Inc/DDSTextureLoader.h"
#include "vendor/DirectXTK/Inc/WICTextureLoader.h"

#include "Tracy/Tracy.hpp"

std::wstring StringToWideString(const std::string& str);
std::string WideStringToString(const std::wstring& wstr);

#define FEATURE_STRING(features, featureName) "\n" + #featureName + ": " + std::to_string(features.featureName)
#define ADAPTER_DESCRIPTION_WSTRING(desc, info) "\n" + #info + ": " + WideStringToString(desc.info)
#define ADAPTER_DESCRIPTION_STRING(desc, info) "\n" + #info + ": " + std::to_string(desc.info)

std::wstring StringToWideString(const std::string& str)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
	return converterX.from_bytes(str);
}

std::string WideStringToString(const std::wstring& wstr)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}

RenderingDevice::RenderingDevice()
{
	BIND_EVENT_MEMBER_FUNCTION(RootexEvents::WindowResized, RenderingDevice::windowResized);

	GFX_ERR_CHECK(CoInitialize(nullptr));
}

Variant RenderingDevice::windowResized(const Event* event)
{
	if (!m_SwapChain || !m_MainRTSRV | !m_MainRTV)
	{
		return true;
	}

	Vector2 size = Extract<Vector2>(event->getData());

	// Unlink all references bound to the swap chain buffers
	m_MainRTSRV.Reset();
	m_MainRTV.Reset();
	unbindRTVs();
	unbindSRVs();

	GFX_ERR_CHECK(m_SwapChain->ResizeBuffers(2, size.x, size.y, DXGI_FORMAT_UNKNOWN, 0));

	// Gather everything back
	createSwapChainBufferViews();
	DXGI_SWAP_CHAIN_DESC sd;
	GFX_ERR_CHECK(m_SwapChain->GetDesc(&sd));
	createDepthStencil(sd, size.x, size.y);
	createOffScreenViews(size.x, size.y);

	return true;
}

RenderingDevice::~RenderingDevice()
{
	m_SwapChain->SetFullscreenState(false, nullptr);
	CoUninitialize();
}

void RenderingDevice::createSwapChainBufferViews()
{
	Microsoft::WRL::ComPtr<ID3D11Resource> backBuffer = nullptr;
	GFX_ERR_CHECK(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf())));
	GFX_ERR_CHECK(m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_MainRTV));
	GFX_ERR_CHECK(m_Device->CreateShaderResourceView(backBuffer.Get(), nullptr, &m_MainRTSRV));
}

void RenderingDevice::createDepthStencil(DXGI_SWAP_CHAIN_DESC& sd, float width, float height)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil = nullptr;
	D3D11_TEXTURE2D_DESC descDepth = { 0 };
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
	descDepth.SampleDesc.Count = sd.SampleDesc.Count;
	descDepth.SampleDesc.Quality = sd.SampleDesc.Quality;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	GFX_ERR_CHECK(m_Device->CreateTexture2D(&descDepth, nullptr, &depthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSView = {};
	descDSView.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSView.Texture2D.MipSlice = 0u;

	GFX_ERR_CHECK(m_Device->CreateDepthStencilView(depthStencil.Get(), &descDSView, &m_MainDSV));

	D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc;
	depthSRVDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	depthSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthSRVDesc.Texture2D.MostDetailedMip = 0;
	depthSRVDesc.Texture2D.MipLevels = 1;

	GFX_ERR_CHECK(m_Device->CreateShaderResourceView(depthStencil.Get(), &depthSRVDesc, &m_MainDSSRV));
}

void RenderingDevice::setScreenState(bool fullscreen)
{
	m_SwapChain->SetFullscreenState(fullscreen, nullptr);
}

void RenderingDevice::initialize(HWND hWnd, int width, int height)
{
	m_WindowHandle = hWnd;
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel = {};

	GFX_ERR_CHECK(D3D11CreateDevice(0, // Default adapter
	    D3D_DRIVER_TYPE_HARDWARE,
	    0, // No software device
	    createDeviceFlags,
	    0,
	    0, // Default feature level array
	    D3D11_SDK_VERSION,
	    &m_Device,
	    &featureLevel,
	    &m_Context));

	PANIC(featureLevel != D3D_FEATURE_LEVEL_11_0, "Direct3D Feature Level 11 is not supported on this hardware.");

	createSwapChainAndRTVs(width, height, hWnd);

	D3D11_FEATURE_DATA_D3D11_OPTIONS features;
	GFX_ERR_CHECK(m_Device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS, &features, sizeof(features)));

	PRINT("**** Supported DirectX11 Features ****"
	    + FEATURE_STRING(features, ClearView)
	    + FEATURE_STRING(features, ConstantBufferOffsetting)
	    + FEATURE_STRING(features, CopyWithOverlap)
	    + FEATURE_STRING(features, DiscardAPIsSeenByDriver)
	    + FEATURE_STRING(features, ExtendedDoublesShaderInstructions)
	    + FEATURE_STRING(features, ExtendedResourceSharing)
	    + FEATURE_STRING(features, FlagsForUpdateAndCopySeenByDriver)
	    + FEATURE_STRING(features, MapNoOverwriteOnDynamicBufferSRV)
	    + FEATURE_STRING(features, MapNoOverwriteOnDynamicConstantBuffer)
	    + FEATURE_STRING(features, MultisampleRTVWithForcedSampleCountOne)
	    + FEATURE_STRING(features, OutputMergerLogicOp)
	    + FEATURE_STRING(features, SAD4ShaderInstructions)
	    + FEATURE_STRING(features, UAVOnlyRenderingForcedSampleCount));

	{
		D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		GFX_ERR_CHECK(m_Device->CreateDepthStencilState(&dsDesc, &m_DSState));
		m_Context->OMSetDepthStencilState(m_DSState.Get(), 1u);
		m_StencilRef = 1u;
	}
	{
		D3D11_DEPTH_STENCIL_DESC dssDesc;
		ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		dssDesc.DepthEnable = true;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		GFX_ERR_CHECK(m_Device->CreateDepthStencilState(&dssDesc, &m_SkyDSState));
	}

	//REMARK- reversed winding order to allow ccw .obj files to be rendered properly, can trouble later
	{
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
		rsDesc.AntialiasedLineEnable = TRUE;

		GFX_ERR_CHECK(m_Device->CreateRasterizerState(&rsDesc, &m_DefaultRS));
	}
	{
		D3D11_RASTERIZER_DESC rsDesc;
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_NONE;
		rsDesc.FrontCounterClockwise = FALSE;
		rsDesc.DepthBias = 0;
		rsDesc.SlopeScaledDepthBias = 0.0f;
		rsDesc.DepthBiasClamp = 0.0f;
		rsDesc.DepthClipEnable = TRUE;
		rsDesc.ScissorEnable = FALSE;
		rsDesc.MultisampleEnable = FALSE;
		rsDesc.AntialiasedLineEnable = TRUE;

		GFX_ERR_CHECK(m_Device->CreateRasterizerState(&rsDesc, &m_SkyRS));
	}
	{
		D3D11_RASTERIZER_DESC rsDesc;
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_NONE;
		rsDesc.FrontCounterClockwise = FALSE;
		rsDesc.DepthBias = 0;
		rsDesc.SlopeScaledDepthBias = 0.0f;
		rsDesc.DepthBiasClamp = 0.0f;
		rsDesc.DepthClipEnable = FALSE;
		rsDesc.ScissorEnable = FALSE;
		rsDesc.MultisampleEnable = FALSE;
		rsDesc.AntialiasedLineEnable = TRUE;

		GFX_ERR_CHECK(m_Device->CreateRasterizerState(&rsDesc, &m_UIRS));
	}
	{
		D3D11_RASTERIZER_DESC rsDesc;
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_NONE;
		rsDesc.FrontCounterClockwise = FALSE;
		rsDesc.DepthBias = 0;
		rsDesc.SlopeScaledDepthBias = 0.0f;
		rsDesc.DepthBiasClamp = 0.0f;
		rsDesc.DepthClipEnable = TRUE;
		rsDesc.ScissorEnable = TRUE;
		rsDesc.MultisampleEnable = FALSE;
		rsDesc.AntialiasedLineEnable = TRUE;

		GFX_ERR_CHECK(m_Device->CreateRasterizerState(&rsDesc, &m_UIScissoredRS));
	}
	{
		D3D11_RASTERIZER_DESC wireframeDesc;
		wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
		wireframeDesc.CullMode = D3D11_CULL_NONE;
		wireframeDesc.FrontCounterClockwise = FALSE;
		wireframeDesc.DepthBias = 0;
		wireframeDesc.SlopeScaledDepthBias = 0.0f;
		wireframeDesc.DepthBiasClamp = 0.0f;
		wireframeDesc.DepthClipEnable = TRUE;
		wireframeDesc.ScissorEnable = FALSE;
		wireframeDesc.MultisampleEnable = FALSE;
		wireframeDesc.AntialiasedLineEnable = TRUE;

		GFX_ERR_CHECK(m_Device->CreateRasterizerState(&wireframeDesc, &m_WireframeRS));
	}
	m_CurrentRS = m_DefaultRS.GetAddressOf();

	m_Context->RSSetState(*m_CurrentRS);

	setOffScreenRTVDSV();

	{
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		D3D11_RENDER_TARGET_BLEND_DESC renderBlendDesc;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		GFX_ERR_CHECK(m_Device->CreateBlendState(&blendDesc, &m_DefaultBS));
	}
	{
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		D3D11_RENDER_TARGET_BLEND_DESC renderBlendDesc;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		GFX_ERR_CHECK(m_Device->CreateBlendState(&blendDesc, &m_AlphaBS));
	}
	m_FontBatch.reset(new DirectX::SpriteBatch(m_Context.Get()));

	setOffScreenRTVDSV();
}

void RenderingDevice::createOffScreenViews(int width, int height)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	GFX_ERR_CHECK(m_Device->CreateTexture2D(&textureDesc, NULL, &m_OffScreenTexture));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	GFX_ERR_CHECK(m_Device->CreateRenderTargetView(m_OffScreenTexture.Get(), &renderTargetViewDesc, &m_OffScreenRTV));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	GFX_ERR_CHECK(m_Device->CreateShaderResourceView(m_OffScreenTexture.Get(), &shaderResourceViewDesc, &m_OffScreenSRV));
}

void RenderingDevice::createSwapChainAndRTVs(int width, int height, const HWND& hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	sd.BufferCount = 2;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice = 0;
	m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter = 0;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	DXGI_ADAPTER_DESC desc;
	dxgiAdapter->GetDesc(&desc);
	PRINT("**** Current Adapter ****"
	    + ADAPTER_DESCRIPTION_STRING(desc, VendorId)
	    + ADAPTER_DESCRIPTION_WSTRING(desc, Description)
	    + ADAPTER_DESCRIPTION_STRING(desc, DeviceId)
	    + ADAPTER_DESCRIPTION_STRING(desc, SubSysId)
	    + ADAPTER_DESCRIPTION_STRING(desc, Revision)
	    + ADAPTER_DESCRIPTION_STRING(desc, DedicatedVideoMemory)
	    + ADAPTER_DESCRIPTION_STRING(desc, DedicatedSystemMemory)
	    + ADAPTER_DESCRIPTION_STRING(desc, SharedSystemMemory));

	Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory = 0;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	dxgiFactory->CreateSwapChain(m_Device.Get(), &sd, &m_SwapChain);

	createDepthStencil(sd, width, height);
	createSwapChainBufferViews();
	createOffScreenViews(width, height);
}

Ref<DirectX::SpriteFont> RenderingDevice::createFont(const String& fontFilePath)
{
	return Ref<DirectX::SpriteFont>(new DirectX::SpriteFont(m_Device.Get(), StringToWideString(fontFilePath.c_str()).c_str()));
}

Microsoft::WRL::ComPtr<ID3DBlob> RenderingDevice::createBlob(LPCWSTR path)
{
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob = nullptr;
	GFX_ERR_CHECK(D3DReadFileToBlob(path, &pBlob));
	return pBlob;
}

void RenderingDevice::enableSkyDSS()
{
	m_Context->OMSetDepthStencilState(m_SkyDSState.Get(), 0);
}

void RenderingDevice::disableSkyDSS()
{
	m_Context->OMSetDepthStencilState(m_DSState.Get(), m_StencilRef);
}

void RenderingDevice::createRTVAndSRV(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
{
	RECT rect;
	GetClientRect(m_WindowHandle, &rect);
	float width = rect.right - rect.left;
	float height = rect.bottom - rect.top;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	GFX_ERR_CHECK(m_Device->CreateTexture2D(&textureDesc, NULL, &texture));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	GFX_ERR_CHECK(m_Device->CreateRenderTargetView(texture.Get(), &renderTargetViewDesc, &rtv));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	GFX_ERR_CHECK(m_Device->CreateShaderResourceView(texture.Get(), &shaderResourceViewDesc, &srv));
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderingDevice::createBuffer(D3D11_BUFFER_DESC* bd, D3D11_SUBRESOURCE_DATA* sd)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(bd, sd, &buffer));
	return buffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderingDevice::createVSCB(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(cbd, csd, &constantBuffer));
	return constantBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderingDevice::createPSCB(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer = nullptr;
	GFX_ERR_CHECK(m_Device->CreateBuffer(cbd, csd, &constantBuffer));
	return constantBuffer;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> RenderingDevice::createPS(ID3DBlob* blob)
{
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader = nullptr;
	GFX_ERR_CHECK(m_Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader));
	return pixelShader;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> RenderingDevice::createVS(ID3DBlob* blob)
{
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader = nullptr;
	GFX_ERR_CHECK(m_Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader));
	return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11InputLayout> RenderingDevice::createVL(ID3DBlob* vertexShaderBlob, const D3D11_INPUT_ELEMENT_DESC* ied, UINT size)
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

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderingDevice::createDDSTexture(const char* imageDDSFileData, size_t size)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> textureResource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;

	if (FAILED(DirectX::CreateDDSTextureFromMemoryEx(m_Device.Get(), (const uint8_t*)imageDDSFileData, size, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, false, &textureResource, &textureView)))
	{
		ERR("Could not load DDS texture from file data");
	}

	return textureView;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderingDevice::createTexture(const char* imageFileData, size_t size)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> textureResource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;

	if (FAILED(DirectX::CreateWICTextureFromMemoryEx(m_Device.Get(), (const uint8_t*)imageFileData, size, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, DirectX::WIC_LOADER_IGNORE_SRGB | DirectX::WIC_LOADER_FORCE_RGBA32, textureResource.GetAddressOf(), textureView.GetAddressOf())))
	{
		ERR("Could not create texture from file data");
	}

	return textureView;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderingDevice::createTextureFromPixels(const char* imageRawData, unsigned int width, unsigned int height)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = imageRawData;
	data.SysMemPitch = width * 4;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	if (FAILED(m_Device->CreateTexture2D(&textureDesc, &data, &texture2D)))
	{
		ERR("Could not create texture 2D from pixel data");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	m_Device->CreateShaderResourceView(texture2D.Get(), &srvDesc, &textureSRV);

	return textureSRV;
}

void RenderingDevice::bind(ID3D11Buffer* const* vertexBuffer, int count, const unsigned int* stride, const unsigned int* offset)
{
	m_Context->IASetVertexBuffers(0u, count, vertexBuffer, stride, offset);
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
		ERR("Could not map to buffer");
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

void RenderingDevice::setVSCB(ID3D11Buffer* constantBuffer, UINT slot)
{
	m_Context->VSSetConstantBuffers(slot, 1u, &constantBuffer);
}

void RenderingDevice::setPSCB(ID3D11Buffer* constantBuffer, UINT slot)
{
	m_Context->PSSetConstantBuffers(slot, 1u, &constantBuffer);
}

void RenderingDevice::unbindSRVs()
{
	ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
	m_Context->PSSetShaderResources(0, 2, nullSRV);
}

void RenderingDevice::unbindRTVs()
{
	m_Context->OMSetRenderTargets(0, nullptr, nullptr);
}

void RenderingDevice::setAlphaBS()
{
	static float blendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_Context->OMSetBlendState(m_AlphaBS.Get(), blendFactors, 0xffffffff);
}

void RenderingDevice::setDefaultBS()
{
	static float blendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_Context->OMSetBlendState(m_DefaultBS.Get(), blendFactors, 0xffffffff);
}

void RenderingDevice::setCurrentRS()
{
	m_Context->RSSetState(*m_CurrentRS);
}

RenderingDevice::RasterizerState RenderingDevice::getRSType()
{
	return m_CurrentRSType;
}

void RenderingDevice::setRSType(RasterizerState rs)
{
	m_CurrentRSType = rs;
	switch (rs)
	{
	case RenderingDevice::RasterizerState::Default:
		m_CurrentRS = m_DefaultRS.GetAddressOf();
		break;
	case RenderingDevice::RasterizerState::UI:
		m_CurrentRS = m_UIRS.GetAddressOf();
		break;
	case RenderingDevice::RasterizerState::UIScissor:
		m_CurrentRS = m_UIScissoredRS.GetAddressOf();
		break;
	case RenderingDevice::RasterizerState::Wireframe:
		m_CurrentRS = m_WireframeRS.GetAddressOf();
		break;
	case RenderingDevice::RasterizerState::Sky:
		m_CurrentRS = m_SkyRS.GetAddressOf();
		break;
	default:
		ERR("Invalid rasterizer state found to be set");
		break;
	}
}

void RenderingDevice::setTemporaryUIRS()
{
	m_Context->RSSetState(m_UIRS.Get());
}

void RenderingDevice::setTemporaryUIScissoredRS()
{
	m_Context->RSSetState(m_UIScissoredRS.Get());
}

void RenderingDevice::setScissorRectangle(int x, int y, int width, int height)
{
	D3D11_RECT rect;
	rect.left = x;
	rect.right = x + width;
	rect.top = y;
	rect.bottom = y + height;

	m_Context->RSSetScissorRects(1, &rect);
}

void RenderingDevice::setResolutionAndRefreshRate(int width, int height, int refreshRateNum, int refreshRateDeno)
{
	DXGI_MODE_DESC dmd;
	ZeroMemory(&dmd, sizeof(DXGI_MODE_DESC));
	dmd.Width = width;
	dmd.Format = DXGI_FORMAT_UNKNOWN;
	DXGI_RATIONAL rr;
	rr.Numerator = refreshRateNum;
	rr.Denominator = refreshRateDeno;
	dmd.RefreshRate = rr;
	dmd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dmd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	GFX_ERR_CHECK(m_SwapChain->ResizeTarget(&dmd));
}

void RenderingDevice::setDSS()
{
	m_Context->OMSetDepthStencilState(m_DSState.Get(), m_StencilRef);
}

void RenderingDevice::setOffScreenRTVDSV()
{
	m_Context->OMSetRenderTargets(1, m_OffScreenRTV.GetAddressOf(), m_MainDSV.Get());
}

void RenderingDevice::setOffScreenRTVOnly()
{
	m_Context->OMSetRenderTargets(1, m_OffScreenRTV.GetAddressOf(), nullptr);
}

void RenderingDevice::setMainRT()
{
	m_Context->OMSetRenderTargets(1, m_MainRTV.GetAddressOf(), nullptr);
}

void RenderingDevice::setRTV(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv)
{
	m_Context->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);
}

void RenderingDevice::setRTV(ID3D11RenderTargetView* rtv)
{
	m_Context->OMSetRenderTargets(1, &rtv, nullptr);
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderingDevice::getMainSRV()
{
	return m_MainRTSRV;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderingDevice::getDepthSSRV()
{
	return m_MainDSSRV;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderingDevice::getOffScreenSRV()
{
	return m_OffScreenSRV;
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
	if (m_Context)
	{
		m_Context->RSSetViewports(1u, vp);
	}
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> RenderingDevice::createSS()
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

Microsoft::WRL::ComPtr<ID3D11SamplerState> RenderingDevice::createSSAnisotropic()
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;
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
	ZoneNamedN(drawCall, "Draw Call", true);
	m_Context->DrawIndexed(number, 0u, 0u);
}

void RenderingDevice::drawIndexedInstanced(UINT indices, UINT instances, UINT startInstance)
{
	ZoneNamedN(drawCall, "Draw Instances Call", true);
	m_Context->DrawIndexedInstanced(indices, instances, 0u, 0u, startInstance);
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
	GFX_ERR_CHECK(m_SwapChain->Present(0, 0));
}

void RenderingDevice::clearRTV(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv, float r, float g, float b, float a)
{
	const float color[] = { r, g, b, a };
	m_Context->ClearRenderTargetView(rtv.Get(), color);
}

void RenderingDevice::clearMainRT(float r, float g, float b, float a)
{
	const float color[] = { r, g, b, a };
	m_Context->ClearRenderTargetView(m_MainRTV.Get(), color);
}

void RenderingDevice::clearOffScreenRT(float r, float g, float b, float a)
{
	const float color[] = { r, g, b, a };
	m_Context->ClearRenderTargetView(m_OffScreenRTV.Get(), color);
}

void RenderingDevice::clearDSV()
{
	m_Context->ClearDepthStencilView(m_MainDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

ID3D11Device* RenderingDevice::getDevice()
{
	return m_Device.Get();
}

ID3D11DeviceContext* RenderingDevice::getContext()
{
	return m_Context.Get();
}
