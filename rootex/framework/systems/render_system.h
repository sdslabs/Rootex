#pragma once

#include "core/renderer/renderer.h"
#include "framework/components/visual/camera_component.h"
#include "framework/system.h"
#include "framework/systems/hierarchy_system.h"
#include "main/window.h"
#include "components/visual/model_component.h"
#include "renderer/render_pass.h"

#include "PostProcess.h"

#define LINE_INITIAL_RENDER_CACHE 1000

class RenderSystem : public System
{
	struct LineRequests
	{
		Vector<float> m_Endpoints;
		Vector<unsigned short> m_Indices;
	};

	CameraComponent* m_Camera;
	
	Ptr<Renderer> m_Renderer;
	Vector<Matrix> m_TransformationStack;

	Ref<BasicMaterial> m_LineMaterial;
	LineRequests m_CurrentFrameLines;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSPerFrameConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSProjectionConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PSPerFrameConstantBuffer;

	bool m_IsEditorRenderPassEnabled;

	Ptr<DirectX::BasicPostProcess> m_BasicPostProcess;
	Ptr<DirectX::DualPostProcess> m_DualPostProcess;
	Ptr<DirectX::ToneMapPostProcess> m_ToneMapPostProcess;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_ToneMapRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ToneMapSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_GaussianBlurRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_GaussianBlurSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_MonochromeRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_MonochromeSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_SepiaRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SepiaSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BloomExtractRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_BloomExtractSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BloomHorizontalBlurRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_BloomHorizontalBlurSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BloomVerticalBlurRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_BloomVerticalBlurSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BloomRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_BloomSRV;

	RenderSystem();
	RenderSystem(RenderSystem&) = delete;
	virtual ~RenderSystem() = default;

	void renderPassRender(RenderPass renderPass);

public:
	static RenderSystem* GetSingleton();
	
	void setConfig(const JSON::json& configData, bool openInEditor) override;
	void update(float deltaMilliseconds) override;
	void renderLines();
	void submitLine(const Vector3& from, const Vector3& to);
	void recoverLostDevice();

	void setCamera(CameraComponent* camera);
	void restoreCamera();

	void calculateTransforms(HierarchyComponent* hierarchyComponent);
	void pushMatrix(const Matrix& transform);
	void pushMatrixOverride(const Matrix& transform);
	void popMatrix();
	
	void enableWireframeRasterizer();
	void resetDefaultRasterizer();

	void setProjectionConstantBuffers();
	void perFrameVSCBBinds(float fogStart, float fogEnd);
	void perFramePSCBBinds(const Color& fogColor);

	void setIsEditorRenderPass(bool enabled) { m_IsEditorRenderPassEnabled = enabled; }
	
	void enableLineRenderMode();
	void resetRenderMode();

	CameraComponent* getCamera() const { return m_Camera; }
	const Matrix& getCurrentMatrix() const;
	const Renderer* getRenderer() const { return m_Renderer.get(); }

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
