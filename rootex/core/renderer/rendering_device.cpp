#include <core/renderer/d3d11graphics.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

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
	    &pSwapChain,
	    &pDevice,
	    nullptr,
	    &pContext);
	ID3D11Resource* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(
	    pBackBuffer,
	    nullptr,
	    &pTarget);
}

void RootexGraphics::EndFrame()
{
	pSwapChain->Present(1u, 0);
}

RootexGraphics::~RootexGraphics()
{
	SafeRelease(&pDevice);
	SafeRelease(&pSwapChain);
	SafeRelease(&pContext);
	SafeRelease(&pTarget);
}

void RootexGraphics::ClearBuffer(float r, float g, float b)
{
	const float color[] = { r, g, b, 1.0f };
	pContext->ClearRenderTargetView(pTarget, color);
}
void RootexGraphics::DrawTestCube(float angle)
{
	struct Vertex
	{
		struct
		{
			float x, y, z;
		} pos;
		struct
		{
			unsigned char r, g, b, a;
		} color;
	};
	const Vertex vertices[] = {
		{ -1.0f, -1.0f, -1.0f, 255, 0, 0},
		{ 1.0f, -1.0f, -1.0f, 0, 255, 0 },
		{ -1.0f, 1.0f, -1.0f, 0, 0, 255 },
		{ 1.0f, 1.0f, -1.0f, 255, 255, 0 },
		{ -1.0f, -1.0f, 1.0f, 255, 0, 255 },
		{ 1.0f, -1.0f, 1.0f, 0, 255, 255 },
		{ -1.0f,1.0f, 1.0f, 0, 0, 0 },
		{ 1.0f, 1.0f, 1.0f, 255, 255, 255 },
	};

	ID3D11Buffer* pVertexBuffer = nullptr;
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

	pDevice->CreateBuffer(&vbd, &vsd, &pVertexBuffer);
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, &pVertexBuffer, &stride, &offset);

	SafeRelease(&pVertexBuffer);

	//create and bind index buffer
	const unsigned short indices[] = {
		0,2,1,
		2,3,1,
		1,3,5,
		3,7,5,
		2,6,3,
		3,6,7,
		4,5,7,
		4,7,6,
		0,4,2,
		2,4,6,
		0,1,4,
		1,5,4
	};
	ID3D11Buffer* pIndexBuffer = nullptr;
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = indices;

	pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);
	pContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u);

	SafeRelease(&pIndexBuffer);

	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};

	//constant buffer(rot matrix) used in vertex shader
	const ConstantBuffer cb = {
		{ DirectX::XMMatrixTranspose(
		    DirectX::XMMatrixRotationZ(angle) * 
		    DirectX::XMMatrixRotationY(angle/2) * 
		    DirectX::XMMatrixRotationX(angle/3) * 
			DirectX::XMMatrixTranslation(0.0f, 0.0f, 4.0f) *
			DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f, 0.5f, 10.0f)
		) 
		}
	};
	ID3D11Buffer* pConstantBuffer = nullptr;
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = { 0 };
	csd.pSysMem = &cb;

	pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);
	pContext->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);

	SafeRelease(&pConstantBuffer);

	ID3DBlob* pBlob = nullptr;

	//load pixel shader
	ID3D11PixelShader* pPixelShader = nullptr;
	D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
	pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
	pContext->PSSetShader(pPixelShader, nullptr, 0u);

	SafeRelease(&pBlob);
	SafeRelease(&pPixelShader);

	//load vertex shader
	ID3D11VertexShader* pVertexShader = nullptr;
	D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
	pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
	pContext->VSSetShader(pVertexShader, nullptr, 0u);

	SafeRelease(&pVertexShader);

	//inout layout 2d
	ID3D11InputLayout* pInputLayout = nullptr;
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	pDevice->CreateInputLayout(
	    ied, (UINT)std::size(ied),
	    pBlob->GetBufferPointer(),
	    pBlob->GetBufferSize(),
	    &pInputLayout);

	SafeRelease(&pBlob);

	//bind vertex layout
	pContext->IASetInputLayout(pInputLayout);

	SafeRelease(&pInputLayout);

	pContext->OMSetRenderTargets(1u, &pTarget, nullptr);

	//set topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//viewport
	D3D11_VIEWPORT vp;
	vp.Width = 640;
	vp.Height = 480;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u);
}

template <class T>
void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}