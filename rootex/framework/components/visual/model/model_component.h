#pragma once

#include "component.h"
#include "renderable_component.h"
#include "components/space/transform_component.h"
#include "core/resource_files/model_resource_file.h"
#include "core/resource_files/basic_material_resource_file.h"
#include "core/renderer/mesh.h"

bool CompareMaterials(const Pair<Ref<MaterialResourceFile>, Vector<Mesh>>& a, const Pair<Ref<MaterialResourceFile>, Vector<Mesh>>& b);

class ModelComponent : public RenderableComponent
{
	DEFINE_COMPONENT(ModelComponent);

protected:
	Ref<ModelResourceFile> m_ModelResourceFile;

	void assignBoundingBox();
	void assignOverrides(Ref<ModelResourceFile> newModel, const HashMap<String, String>& materialOverrides);

public:
	ModelComponent(
	    unsigned int renderPass,
	    Ref<ModelResourceFile> resFile,
	    const HashMap<String, String>& materialOverrides,
	    bool isVisible,
	    bool lodEnable,
	    float lodBias,
	    float lodDistance,
	    const Vector<SceneID>& affectingStaticLightIDs);
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
