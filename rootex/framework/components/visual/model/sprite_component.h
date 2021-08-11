#pragma once

#include "component.h"
#include "renderable_component.h"
#include "core/resource_files/image_resource_file.h"
#include "core/resource_files/basic_material_resource_file.h"
#include "core/renderer/mesh.h"

class SpriteComponent : public RenderableComponent
{
	COMPONENT(SpriteComponent, Category::Model);

	Ref<ImageResourceFile> m_ImageResourceFile;

	Ref<BasicMaterialResourceFile> m_ImageMaterial;
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;

	void configureImageMaterial();
	void configureBoundingRectangle();

public:
	SpriteComponent(Entity& owner, const JSON::json& data);
	~SpriteComponent() = default;

	bool preRender(float deltaMilliseconds) override;
	void render(float viewDistance) override;

	void setImageResourceFile(Ref<ImageResourceFile> newImage);
	ImageResourceFile* getImageResourceFile() const { return m_ImageResourceFile.get() ; }

	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(SpriteComponent);
