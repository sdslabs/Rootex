#pragma once

#include <d3d11.h>

#include "common/common.h"
#include "core/renderer/renderer.h"
#include "framework/entity.h"
#include "scene/scene_node.h"
#include "scene/camera_node.h"
#include "scene/root_node.h"

class Scene
{
protected:
	Ptr<RootNode> m_Root;
	Ref<CameraNode> m_Camera;
	Ptr<Renderer> m_Renderer;
	Vector<Matrix> m_TransformationStack;
	HashMap<EntityID, Ref<SceneNode>> m_SceneEntityNodeMap;

public:
	Scene(int width, int height);
	Scene(Scene&) = delete;
	~Scene();

	void render();
	bool load();
	void recoverLostDevice();
	bool update(float deltaMilliseconds);

	bool addChild(EntityID id, Ref<SceneNode> child);
	bool removeChild(EntityID id);
	SceneNode* findNode(EntityID id);

	void setCamera(Ref<CameraNode> camera);

	void pushMatrix(const Matrix& transform);
	void popMatrix();
	
	const CameraNode* getCamera() const { return m_Camera.get(); }
	const Matrix* getTopMatrix() const;
	const Renderer* getRenderer() const { return m_Renderer.get(); }
};
