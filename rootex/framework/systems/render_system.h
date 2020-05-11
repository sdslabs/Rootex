#pragma once

#include "core/renderer/renderer.h"
#include "framework/components/visual/camera_component.h"
#include "framework/system.h"
#include "framework/systems/hierarchy_system.h"
#include "main/window.h"
#include "components/visual/model_component.h"
#include "renderer/render_pass.h"

class RenderSystem : public System
{
	CameraComponent* m_Camera;

	Ptr<Renderer> m_Renderer;
	Vector<Matrix> m_TransformationStack;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSPerFrameConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSProjectionConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PSPerFrameConstantBuffer;

	bool m_IsEditorRenderPassEnabled;

	RenderSystem();
	RenderSystem(RenderSystem&) = delete;
	~RenderSystem() = default;

	void renderPassRender(RenderPass renderPass);

public:
	static RenderSystem* GetSingleton();
	
	void render();
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
	void perFrameVSCBBinds();
	void perFramePSCBBinds();

	void setIsEditorRenderPass(bool enabled) { m_IsEditorRenderPassEnabled = enabled; }
	
	void enableLineRenderMode();
	void resetRenderMode();

	CameraComponent* getCamera() const { return m_Camera; }
	const Matrix& getCurrentMatrix() const;
	const Renderer* getRenderer() const { return m_Renderer.get(); }
};
