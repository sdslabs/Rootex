#pragma once

#include "core/renderer/renderer.h"
#include "core/renderer/render_pass.h"
#include "core/resource_files/basic_material_resource_file.h"
#include "main/window.h"
#include "framework/components/visual/camera_component.h"
#include "components/visual/model/model_component.h"
#include "components/visual/model/animated_model_component.h"
#include "components/visual/model/sprite_component.h"
#include "transform_system.h"
#include "components/space/transform_component.h"

#include "ASSAO/ASSAO.h"

/// Responsible for handling all the rendering in the editor.
class RenderSystem : public System
{
	EventBinder<RenderSystem> m_Binder;

	struct LineRequests
	{
		Vector<float> m_Endpoints;
		Vector<unsigned short> m_Indices;
	};

	CameraComponent* m_Camera;

	Ptr<Renderer> m_Renderer;

	Ref<BasicMaterialResourceFile> m_LineMaterial;
	LineRequests m_CurrentFrameLines;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PerFrameVSCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PerCameraChangeVSCB;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PerCameraChangePSCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PerFrameCustomPSCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PerFramePSCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PerScenePSCB;

	bool m_IsEditorRenderPassEnabled;

	RenderSystem();
	RenderSystem(RenderSystem&) = delete;

	void renderPassRender(float deltaMilliseconds, RenderPass renderPass);

	Variant onOpenedScene(const Event* event);

public:
	static RenderSystem* GetSingleton();

	void setConfig(const SceneSettings& sceneSettings) override;
	void update(float deltaMilliseconds) override;
	void renderLines();

	void submitLine(const Vector3& from, const Vector3& to);

	void submitBox(const Vector3& min, const Vector3& max);
	void submitSphere(const Vector3& center, const float& radius);
	void submitCone(const Matrix& transform, const float& height, const float& radius);

	void recoverLostDevice();
	void getDirectionalLightComponent();

	Matrix m_ViewMatrixForShadowRender;
	Matrix setViewMatrixForShadowRender();
	void refreshViewMatrixForShadowRender();

	void setCamera(CameraComponent* camera);
	void restoreCamera();

	void enableWireframeRasterizer();
	void resetDefaultRasterizer();

	void setPerCameraVSCBs();
	void setPerFrameVSCBs(float fogStart, float fogEnd);
	void setPerCameraChangePSCBs();
	void setPerFramePSCBs(const Color& fogColor);
	void setPerScenePSCBs();
	void updateStaticLights();
	void updatePerSceneBinds();

	void setIsEditorRenderPass(bool enabled) { m_IsEditorRenderPassEnabled = enabled; }

	void enableLineRenderMode();
	void resetRenderMode();

	CameraComponent* getCamera() const { return m_Camera; }
	Renderer* getRenderer() const { return m_Renderer.get(); }

	void draw() override;
};
