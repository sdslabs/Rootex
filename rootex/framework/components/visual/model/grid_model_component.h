#pragma once

#include "model_component.h"
#include "renderer/render_pass.h"
#include "core/resource_files/basic_material_resource_file.h"

class GridModelComponent : public ModelComponent
{
	DEFINE_COMPONENT(GridModelComponent);

	Ref<BasicMaterialResourceFile> m_ColorMaterial;
	Ptr<VertexBuffer> m_VertexBuffer;
	Ptr<IndexBuffer> m_IndexBuffer;

	Vector2 m_CellSize;
	int m_CellCount;

	void refreshVertexBuffers();

public:
	GridModelComponent(const Vector2& cellSize, const int& cellCount, const unsigned int& renderPass, bool isVisible);
	~GridModelComponent() = default;

	void render(float viewDistance) override;

	bool setupData() override;
	JSON::json getJSON() const override;
	void draw() override;
};
