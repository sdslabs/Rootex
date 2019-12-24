#pragma once

#include <d3d11.h>

#include "common/common.h"
#include "core/renderer/renderer.h"
#include "framework/entity.h"
#include "scene/scene_node.h"
#include "scene/camera_node.h"

class Scene
{
protected:
	Ptr<SceneNode> m_Root;
	Ptr<CameraNode> m_Camera;
	Ptr<Renderer> m_Renderer;
	Vector<DirectX::XMMATRIX> m_TransformationStack;
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

	void setCamera(Ref<CameraNode> camera) { m_Camera = std::make_unique<CameraNode>(camera); }
	const CameraNode* getCamera() const { return m_Camera.get(); }

	void pushMatrix(const AlignedMatrix& transform);
	void popMatrix();
	const DirectX::XMMATRIX* getTopMatrix() const;
};
