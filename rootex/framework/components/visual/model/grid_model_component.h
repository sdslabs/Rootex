#pragma once

#include "model_component.h"
#include "renderer/render_pass.h"
#include "core/resource_files/basic_material_resource_file.h"

/// Renders a grid uptil the view distance.
class GridModelComponent : public ModelComponent
{
	COMPONENT(GridModelComponent, Category::Model);

	Ref<BasicMaterialResourceFile> m_ColorMaterial;
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;

	Vector2 m_CellSize;
	int m_CellCount;

	void refreshVertexBuffers();

public:
	GridModelComponent(Entity& owner, const JSON::json& data);
	~GridModelComponent() = default;

	void render(float viewDistance) override;

	bool setupData() override;
	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(GridModelComponent);
