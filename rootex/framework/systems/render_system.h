#pragma once

#include "framework/components/visual/camera_visual_component.h"
#include "framework/system.h"
#include "framework/systems/hierarchy_system.h"
#include "main/window.h"

class RenderSystem : public System
{
	Ref<CameraVisualComponent> m_Camera;
	Ptr<Renderer> m_Renderer;
	Vector<Matrix> m_TransformationStack;

	RenderSystem();
	RenderSystem(RenderSystem&) = delete;
	~RenderSystem();

	friend class VisualComponent;

public:
	static RenderSystem* GetSingleton();
	
	void render();
	void recoverLostDevice();

	void setCamera(Ref<CameraVisualComponent> camera);

	void pushMatrix(const Matrix& transform);
	void popMatrix();

	CameraVisualComponent* getCamera() const { return m_Camera.get(); }
	const Matrix& getTopMatrix() const;
	const Renderer* getRenderer() const { return m_Renderer.get(); }
};
