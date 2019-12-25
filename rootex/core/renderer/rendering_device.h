#pragma once

#include <d3d11.h>

#include <d3dcompiler.h>
#include <string>

class RenderingDevice
{
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_Context;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;

	ID3D11DepthStencilState* m_OldSkyDepthStencilState;
	UINT m_StencilRef;
	ID3D11DepthStencilState* m_NewSkyDepthStencilState;

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

	ID3DBlob* createBlob(LPCWSTR path);
	void enableSkyDepthStencilState();
	void disableSkyDepthStencilState();
	ID3D11Buffer* initVertexBuffer(D3D11_BUFFER_DESC* vbd, D3D11_SUBRESOURCE_DATA* vsd, const UINT* stride, const UINT* offset);
	ID3D11Buffer* initIndexBuffer(D3D11_BUFFER_DESC* ibd, D3D11_SUBRESOURCE_DATA* isd, DXGI_FORMAT format);
	void initVSTransformConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd);
	void initVSConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd);
	void initPSConstantBuffer(D3D11_BUFFER_DESC* cbd, D3D11_SUBRESOURCE_DATA* csd);
	ID3D11PixelShader* initPixelShader(ID3DBlob* blob);
	ID3D11VertexShader* initVertexShader(ID3DBlob* blob);
	void initVertexLayout(ID3DBlob* vertexShaderBlob, const D3D11_INPUT_ELEMENT_DESC* ied, UINT size);
	
	void bind(ID3D11Buffer* vertexBuffer, const unsigned int* stride, const unsigned int* offset);
	void bind(ID3D11Buffer* indexBuffer, DXGI_FORMAT format);
	void bind(ID3D11VertexShader* vertexShader);
	void bind(ID3D11PixelShader* pixelShader);

	void unbindShaderResources();

	void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY pt);
	void setViewport(const D3D11_VIEWPORT* vp);
	
	void drawIndexed(UINT number);
	void clearBuffer(float r, float g, float b);
};
