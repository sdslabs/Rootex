#include "sprite_component.h"

#include "common/common.h"

#include "event_manager.h"
#include "core/resource_loader.h"
#include "systems/render_system.h"

DEFINE_COMPONENT(SpriteComponent);

JSON::json SpriteComponent::InjectSpriteModel(const JSON::json& data)
{
	JSON::json newData = JSON::json(data);
	newData["resFile"] = "rootex/assets/square.obj";
	return newData;
}

SpriteComponent::SpriteComponent(Entity& owner, const JSON::json& data)
    : ModelComponent(owner, SpriteComponent::InjectSpriteModel(data))
    , m_IsBillboarded(data.value("isBillboarded", false))
{
}

bool SpriteComponent::setupData()
{
	bool status = ModelComponent::setupData();

	if (status)
	{
		adjustScaling();
	}

	return status;
}

void SpriteComponent::adjustScaling()
{
	float defaultHeight = getTransformComponent()->getAbsoluteScale().y;

	Ref<GPUTexture> imageTexture = getOverridingMaterialResourceFile()->getTextures()[0];
	float aspectRatio = imageTexture->getWidth() / imageTexture->getHeight();

	getTransformComponent()->setScale(Vector3 { aspectRatio * defaultHeight, defaultHeight, 1.0f });
}

bool SpriteComponent::preRender(float deltaMilliseconds)
{
	ModelComponent::preRender(deltaMilliseconds);

	if (m_IsBillboarded)
	{
		const Vector3& scaling = getTransformComponent()->getAbsoluteScale();

		CameraComponent* renderCamera = RenderSystem::GetSingleton()->getCamera();
		Matrix& cameraTransform = renderCamera->getTransformComponent()->getAbsoluteTransform();

		Matrix billboardMatrix = Matrix::CreateBillboard(
		    getTransformComponent()->getAbsolutePosition(),
		    cameraTransform.Translation(),
		    cameraTransform.Up(),
		    &cameraTransform.Forward());

		billboardMatrix.Right(billboardMatrix.Left());
		billboardMatrix.Forward(billboardMatrix.Backward());

		billboardMatrix.Right(billboardMatrix.Right() * scaling.x);
		billboardMatrix.Up(billboardMatrix.Up() * scaling.y);
		billboardMatrix.Backward(billboardMatrix.Backward() * scaling.z);

		TransformSystem::GetSingleton()->pushMatrixOverride(billboardMatrix);
	}

	return true;
}

void SpriteComponent::postRender()
{
	if (m_IsBillboarded)
	{
		TransformSystem::GetSingleton()->popMatrix();
	}
	ModelComponent::postRender();
}

Ref<MaterialResourceFile> SpriteComponent::getOverridingMaterialResourceFile()
{
	auto& [material, meshes] = getMeshes()[0];
	return m_MaterialOverrides.at(material.get());
}

void SpriteComponent::setMaterialOverride(MaterialResourceFile* oldMaterial, Ref<MaterialResourceFile> newMaterial)
{
	RenderableComponent::setMaterialOverride(oldMaterial, newMaterial);
	adjustScaling();
}

JSON::json SpriteComponent::getJSON() const
{
	JSON::json j = ModelComponent::getJSON();

	j["isBillboarded"] = m_IsBillboarded;

	return j;
}

void SpriteComponent::draw()
{
	ModelComponent::draw();

	ImGui::Checkbox("Billboarded", &m_IsBillboarded);
}
