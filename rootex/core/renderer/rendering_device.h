#pragma once

#include "common/common.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#include "vendor/DirectXTK/Inc/SpriteBatch.h"
#include "vendor/DirectXTK/Inc/SpriteFont.h"

/// The boss of all rendering, all DirectX API calls requiring the Device or Context go through this
class RenderingDevice
{
public:
	enum class RasterizerState
	{
		Default,
		UI,
		UIScissor,
		Wireframe,
		Sky
	};

	enum class SamplerState
	{
		Default,
		Anisotropic
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;

	HWND m_WindowHandle;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_MainRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_MainRTSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_MainDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_MainDSSRV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_OffScreenTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_OffScreenRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_OffScreenSRV;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DSState;
	UINT m_StencilRef;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_SkyDSState;

	Ref<DirectX::SpriteBatch> m_FontBatch;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_DefaultRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_UIRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_UIScissoredRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_WireframeRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_SkyRS;
	ID3D11RasterizerState** m_CurrentRS;
	RasterizerState m_CurrentRSType;

	Microsoft::WRL::ComPtr<ID3D11BlendState> m_DefaultBS;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_AlphaBS;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;

	RenderingDevice();
	RenderingDevice(RenderingDevice&) = delete;
	~RenderingDevice();

	void createSwapChainBufferViews();
	void createDepthStencil(DXGI_SWAP_CHAIN_DESC& sd, float width, float height);

	/// Should only be called by Window class
	void swapBuffers();

	Variant windowResized(const Event* event);

	friend class Window;

public:
	static RenderingDevice* GetSingleton();
	void initialize(HWND hWnd, int width, int height);
	void createOffScreenViews(int width, int height);
	/// Create resources which depend on window height and width
	void createSwapChainAndRTVs(int width, int height, const HWND& hWnd);
	void setScreenState(bool fullscreen);

	ID3D11Device* getDevice();
	ID3D11DeviceContext* getContext();

	void enableSkyDSS();
	void disableSkyDSS();

	void createRTVAndSRV(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv);

	Microsoft::WRL::ComPtr<ID3D11Buffer> createBuffer(const char* data, size_t size, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, int cpuAccess);
	void editBuffer(const char* data, size_t byteSize, ID3D11Buffer* bufferPointer);

	template <class T>
	Microsoft::WRL::ComPtr<ID3D11Buffer> createBuffer(const T& data, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, D3D11_CPU_ACCESS_FLAG cpuAccess);
	template <typename T>
	void editBuffer(const T& data, ID3D11Buffer* bufferPointer);

	/// To hold shader blobs loaded from the compiled shader files
	Microsoft::WRL::ComPtr<ID3DBlob> compileShader(const String& shaderPath, const char* entryPoint, const char* profile);
	Microsoft::WRL::ComPtr<ID3D11PixelShader> createPS(ID3DBlob* blob);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> createVS(ID3DBlob* blob);
	Microsoft::WRL::ComPtr<ID3D11InputLayout> createVL(ID3DBlob* vertexShaderBlob, const D3D11_INPUT_ELEMENT_DESC* ied, UINT size);

	Ref<DirectX::SpriteFont> createFont(const String& fontFilePath);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> createDDSTexture(const char* imageDDSFileData, size_t size);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> createTexture(const char* imageFileData, size_t size);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> createTextureFromPixels(const char* imageRawData, unsigned int width, unsigned int height);
	Microsoft::WRL::ComPtr<ID3D11SamplerState> createSS(SamplerState type);

	void setVSSRV(unsigned int slot, unsigned int count, ID3D11ShaderResourceView** texture);
	void setPSSRV(unsigned int slot, unsigned int count, ID3D11ShaderResourceView** texture);
	void setVSSS(unsigned int slot, unsigned int count, ID3D11SamplerState** samplerState);
	void setPSSS(unsigned int slot, unsigned int count, ID3D11SamplerState** samplerState);
	void setVSCB(unsigned int slot, unsigned int count, ID3D11Buffer** constantBuffer);
	void setPSCB(unsigned int slot, unsigned int count, ID3D11Buffer** constantBuffer);

	void bind(ID3D11Buffer* const* vertexBuffer, int count, const unsigned int* stride, const unsigned int* offset);
	void bind(ID3D11Buffer* indexBuffer, DXGI_FORMAT format);
	void bind(ID3D11VertexShader* vertexShader);
	void bind(ID3D11PixelShader* pixelShader);
	void bind(ID3D11InputLayout* inputLayout);

	void mapBuffer(ID3D11Buffer* buffer, D3D11_MAPPED_SUBRESOURCE& subresource);
	void unmapBuffer(ID3D11Buffer* buffer);
	void setDefaultBS();
	void setAlphaBS();

	void setCurrentRS();
	RasterizerState getRSType();
	void setRSType(RasterizerState rs);

	void setTemporaryUIRS();
	void setTemporaryUIScissoredRS();

	void setDSS();

	void setScissorRectangle(int x, int y, int width, int height);

	void setResolutionAndRefreshRate(int width, int height, int refreshRateNum, int refreshRateDeno);

	void setOffScreenRTVDSV();
	void setOffScreenRTVOnly();
	void setMainRT();
	void setRTV(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv);
	void setRTV(ID3D11RenderTargetView* rtv);

	void unbindSRVs();
	void unbindRTVs();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getMainSRV();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getDepthSSRV();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getOffScreenSRV();

	Ref<DirectX::SpriteBatch> getUIBatch();

	void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY pt);
	void setViewport(const D3D11_VIEWPORT* vp);

	/// The last boss, draws Triangles
	void drawIndexed(UINT indices);
	void drawIndexedInstanced(UINT indices, UINT instances, UINT startInstance);

	void beginDrawUI();
	void endDrawUI();

	void clearRTV(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv, float r, float g, float b, float a);
	void clearMainRT(float r, float g, float b, float a);
	void clearOffScreenRT(float r, float g, float b, float a);
	void clearDSV();
};

template <class T>
inline Microsoft::WRL::ComPtr<ID3D11Buffer> RenderingDevice::createBuffer(const T& data, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, D3D11_CPU_ACCESS_FLAG cpuAccess)
{
	return createBuffer((const char*)&data, sizeof(T), bindFlags, usage, cpuAccess);
}

template <typename T>
inline void RenderingDevice::editBuffer(const T& data, ID3D11Buffer* bufferPointer)
{
	editBuffer((const char*)&data, sizeof(T), bufferPointer);
}
