#include <core/renderer/d3d11graphics.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
RootexGraphics::RootexGraphics(HWND windowHandler, unsigned int w, unsigned int h)
{
	width = w;
	height = h;

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

	//TODO- add HRESULT error check and Device Removed exception

	GFX_ERR_CHECK( D3D11CreateDeviceAndSwapChain(
	    nullptr,
	    D3D_DRIVER_TYPE_HARDWARE,
	    nullptr,
	    D3D11_CREATE_DEVICE_DEBUG, //to enable debug layer diagnostics
	    nullptr,
	    0,
	    D3D11_SDK_VERSION,
	    &descriptor,
	    &pSwapChain,
	    &pDevice,
	    nullptr,
	    &pContext));
	ID3D11Resource* pBackBuffer = nullptr;
	GFX_ERR_CHECK(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));
	GFX_ERR_CHECK(pDevice->CreateRenderTargetView(
	    pBackBuffer,
	    nullptr,
<<<<<<< HEAD
<<<<<<< HEAD
	    &m_Context);
	    &pTarget
	);
}

RootexGraphics::~RootexGraphics()
{
	if (pTarget != nullptr)
		pTarget->Release();
	if (pContext != nullptr)
		pContext->Release();
	if (pSwapChain != nullptr)
		pSwapChain->Release();
	if (pDevice != nullptr)
		pDevice->Release();
=======
	    &pTarget);
=======
	    &pTarget));
>>>>>>> add error-check on D3D11 Device functions

	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ID3D11DepthStencilState* pDSState;
	GFX_ERR_CHECK(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	pContext->OMSetDepthStencilState(pDSState, 1u);

	SafeRelease(&pDSState);
	
	ID3D11Texture2D* pDepthStencil = nullptr;
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
	GFX_ERR_CHECK(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSView = { };
	descDSView.Format = DXGI_FORMAT_D32_FLOAT;
	descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSView.Texture2D.MipSlice = 0u;

	GFX_ERR_CHECK(pDevice->CreateDepthStencilView(pDepthStencil, &descDSView, &pDSView));

	SafeRelease(&pDepthStencil);

	pContext->OMSetRenderTargets(1u, &pTarget, pDSView);

>>>>>>> add depth buffer
}
=======
#include "renderer/d3d11rendering_device.h"
>>>>>>> Refactor context and device away from window
=======
//#include "renderer/d3d11rendering_device.h"
>>>>>>> shift gameLoop() out of Window
=======
#include "renderer/rendering_device.h"
>>>>>>> cubes rendering in partially abstracted architecture

RootexGraphics::RootexGraphics(unsigned int w, unsigned int h)
{
	width = w;
	height = h;
	m_StartTime = std::chrono::system_clock::now();
}

RootexGraphics::~RootexGraphics()
{
}

void RootexGraphics::drawTest()
{
	// The color change effect ->
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - m_StartTime;
	float seconds = elapsed_seconds.count();
	float r = (sin(seconds) + 1.0) * 0.5;
	float g = (sin(seconds * 0.3) + 1.0) * 0.5;
	float b = (sin(seconds * 0.7) + 1.0) * 0.5;
	RenderingDevice::GetSingleton()->clearBuffer(r, g, b);

	drawTestCube(seconds);
	drawTestCube(-seconds);
}

void RootexGraphics::drawTestCube(float angle)
{
	struct Vertex
	{
		struct
		{
			float x, y, z;
		} pos;
	};
	const Vertex vertices[] = {
		{ -1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ -1.0f, 1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
	};

	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.CPUAccessFlags = 0u;
	vbd.MiscFlags = 0u;
	vbd.ByteWidth = sizeof(vertices);
	vbd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA vsd = { 0 };
	vsd.pSysMem = vertices;

	//create and bind vertex buffer

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;

	RenderingDevice::GetSingleton()->initVertexBuffer(&vbd, &vsd, &stride, &offset);

	//create and bind index buffer
	const unsigned short indices[] = {
		0, 2, 1,
		2, 3, 1,
		1, 3, 5,
		3, 7, 5,
		2, 6, 3,
		3, 6, 7,
		4, 5, 7,
		4, 7, 6,
		0, 4, 2,
		2, 4, 6,
		0, 1, 4,
		1, 5, 4
	};
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = indices;

	RenderingDevice::GetSingleton()->initIndexBuffer(&ibd, &isd, DXGI_FORMAT_R16_UINT);

	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};

	//constant buffer(rot matrix) used in vertex shader
	const ConstantBuffer cb = {
		{ DirectX::XMMatrixTranspose(
		    DirectX::XMMatrixRotationZ(angle) * DirectX::XMMatrixRotationY(angle / 2) * DirectX::XMMatrixRotationX(angle / 3) * DirectX::XMMatrixTranslation(0.0f, 0.0f, 4.0f) * DirectX::XMMatrixPerspectiveLH(maxX, maxX * height / width, minZ, maxZ)) }
	};
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = { 0 };
	csd.pSysMem = &cb;

	RenderingDevice::GetSingleton()->initVSConstantBuffer(&cbd, &csd);

	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};

	const ConstantBuffer2 cb2 = {
		{ { 1.0f, 0.0f, 0.0f },
		    { 0.0f, 1.0f, 0.0f },
		    { 0.0f, 0.0f, 1.0f },
		    { 1.0f, 1.0f, 0.0f },
		    { 1.0f, 0.0f, 1.0f },
		    { 0.0f, 1.0f, 1.0f } }
	};

	D3D11_BUFFER_DESC cbd2 = { 0 };
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DYNAMIC;
	cbd2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd2.MiscFlags = 0u;
	cbd2.ByteWidth = sizeof(cb2);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd2 = { 0 };
	csd2.pSysMem = &cb2;

	RenderingDevice::GetSingleton()->initPSConstantBuffer(&cbd2, &csd2);

	RenderingDevice::GetSingleton()->initPixelShader(L"PixelShader.cso");

	ID3DBlob* vertrxShaderBlob = RenderingDevice::GetSingleton()->initVertexShader(L"VertexShader.cso");

	//inout layout 2d
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	RenderingDevice::GetSingleton()->initVertexLayout(vertrxShaderBlob, ied, std::size(ied));

	//set topology
	RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//viewport
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	
	RenderingDevice::GetSingleton()->setViewports(&vp);

	RenderingDevice::GetSingleton()->drawIndexed(std::size(indices));
}