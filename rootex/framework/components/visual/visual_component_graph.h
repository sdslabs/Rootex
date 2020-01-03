#pragma once

#include "common/common.h"
#include "root_visual_component.h"
#include "camera_visual_component.h"
#include "renderer/renderer.h"

class VisualComponentGraph
{
protected:
	Ptr<RootVisualComponent> m_Root;
	Ref<CameraVisualComponent> m_Camera;
	Ptr<Renderer> m_Renderer;
	Vector<Matrix> m_TransformationStack;

public:
	VisualComponentGraph(int width, int height);
	VisualComponentGraph(VisualComponentGraph&) = delete;
	~VisualComponentGraph();

	void render();
	bool load();
	void recoverLostDevice();
	
	bool addChild(VisualComponent* child);
	bool removeChild(Ref<VisualComponent> child);

	void setCamera(Ref<CameraVisualComponent> camera);

	void pushMatrix(const Matrix& transform);
	void popMatrix();

	const CameraVisualComponent* getCamera() const { return m_Camera.get(); }
	const Matrix& getTopMatrix() const;
	const Renderer* getRenderer() const { return m_Renderer.get(); }
};
