#pragma once

#include "component.h"
#include "renderable_component.h"
#include "core/resource_files/image_resource_file.h"
#include "core/resource_files/basic_material_resource_file.h"
#include "core/renderer/mesh.h"

class SpriteComponent : public RenderableComponent
{
	COMPONENT(SpriteComponent, Category::Model);

	Ref<BasicMaterialResourceFile> m_ImageMaterialResourceFile;
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;

	void configureBoundingRectangle();

public:
	SpriteComponent(Entity& owner, const JSON::json& data);
	~SpriteComponent() = default;

	bool preRender(float deltaMilliseconds) override;
	void render(float viewDistance) override;

	void setImageMaterialResourceFile(Ref<BasicMaterialResourceFile> newMaterial);
	BasicMaterialResourceFile* getImageMaterialResourceFile() const { return m_ImageMaterialResourceFile.get() ; }

	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(SpriteComponent);
