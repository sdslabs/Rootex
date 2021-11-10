#pragma once

#include "components/visual/model/model_component.h"
#include "renderer/shader.h"

class DecalComponent : public ModelComponent
{
	COMPONENT(DecalComponent, Category::Effect);

private:
	static inline Ptr<Shader> m_DecalShader;

	void initDecalShader();
	void bindDepthTexture();

public:
	DecalComponent(Entity& owner, const JSON::json& data);
	~DecalComponent() = default;

	void render(float viewDistance) override;

	void draw() override;
};

DECLARE_COMPONENT(DecalComponent);
