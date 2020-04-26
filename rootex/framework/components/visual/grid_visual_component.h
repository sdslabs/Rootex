#pragma once

#include "visual_component.h"
#include "renderer/materials/grid_material.h"

class GridVisualComponent : public VisualComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	Ref<GridMaterial> m_GridMaterial;
	Ptr<VertexBuffer> m_VertexBuffer;
	Ptr<IndexBuffer> m_IndexBuffer;

	Vector2 m_CellSize;
	int m_CellCount;

	GridVisualComponent(const Vector2& cellSize, const long& cellCount, const unsigned int& renderPass, bool isVisible);
	GridVisualComponent(GridVisualComponent&) = delete;
	virtual ~GridVisualComponent() = default;

	friend class EntityFactory;

	void refreshVertexBuffers();
	
public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::GridVisualComponent;

	virtual bool setup() override;
	void render(RenderPass renderPass) override;

	virtual String getName() const override { return "GridVisualComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};