#pragma once

#include "component.h"
#include "renderable_component.h"
#include "components/space/transform_component.h"
#include "core/resource_files/model_resource_file.h"
#include "core/resource_files/basic_material_resource_file.h"
#include "core/renderer/mesh.h"

bool CompareMaterials(const Pair<Ref<MaterialResourceFile>, Vector<Mesh>>& a, const Pair<Ref<MaterialResourceFile>, Vector<Mesh>>& b);

/// Used to render static 3D models.
class ModelComponent : public RenderableComponent
{
	COMPONENT(ModelComponent, Category::Model);

protected:
	Ref<ModelResourceFile> m_ModelResourceFile;

	void assignBoundingBox();
	void assignOverrides(Ref<ModelResourceFile> newModel, const HashMap<String, String>& materialOverrides);

public:
	ModelComponent(Entity& owner, const JSON::json& data);
	virtual ~ModelComponent() = default;

	bool preRender(float deltaMilliseconds) override;
	void render(float viewDistance) override;

	void setModelResourceFile(Ref<ModelResourceFile> newModel, const HashMap<String, String>& materialOverrides);
	ModelResourceFile* getModelResourceFile() const { return m_ModelResourceFile.get(); }

	const Vector<Pair<Ref<BasicMaterialResourceFile>, Vector<Mesh>>>& getMeshes() const { return m_ModelResourceFile->getMeshes(); }

	bool setupData() override;
	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(ModelComponent);
