#pragma once

#include "component.h"
#include "renderable_component.h"
#include "core/resource_files/image_resource_file.h"
#include "core/resource_files/basic_material_resource_file.h"
#include "core/renderer/mesh.h"

class SpriteComponent : public Component
{
	COMPONENT(SpriteComponent, Category::Model);
	DEPENDS_ON(TransformComponent);

	bool m_IsVisible;
	unsigned int m_RenderPass;

	Ref<ImageResourceFile> m_ImageResourceFile;

	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;

	void configureBoundingRectangle();

public:
	SpriteComponent(Entity& owner, const JSON::json& data);
	~SpriteComponent() = default;

	void setVisible(bool enabled);
	bool isVisible() const;

	bool preRender(float deltaMilliseconds);
	void render(float viewDistance);
	void postRender();

	void setImageResourceFile(Ref<ImageResourceFile> newImage);
	ImageResourceFile* getImageResourceFile() const { return m_ImageResourceFile.get() ; }

	unsigned int getRenderPass() const { return m_RenderPass; }

	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(SpriteComponent);
