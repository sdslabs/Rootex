#pragma once

#include "common/common.h"
#include "camera_visual_component.h"
#include "entity_factory.h"
#include "framework/components/root_hierarchy_component.h"
#include "renderer/renderer.h"

class VisualComponentGraph
{
protected:
	Ref<Entity> m_Root;
	Ref<RootHierarchyComponent> m_RootHierarchyComponent;
	Ref<CameraVisualComponent> m_Camera;
	Ptr<Renderer> m_Renderer;
	Vector<Matrix> m_TransformationStack;

public:
	VisualComponentGraph();
	VisualComponentGraph(VisualComponentGraph&) = delete;
	~VisualComponentGraph();

	void render();
	void recoverLostDevice();
	
	bool addChild(Ref<Entity> child);
	bool removeChild(Ref<Entity> child);

	void setCamera(Ref<CameraVisualComponent> camera);

	void pushMatrix(const Matrix& transform);
	void popMatrix();

	CameraVisualComponent* getCamera() const { return m_Camera.get(); }
	const Matrix& getTopMatrix() const;
	const Renderer* getRenderer() const { return m_Renderer.get(); }
};
