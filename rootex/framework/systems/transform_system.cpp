#include "transform_system.h"

#include "core/resource_loader.h"
#include "components/visual/effect/fog_component.h"
#include "components/visual/effect/sky_component.h"
#include "components/visual/model/grid_model_component.h"
#include "components/visual/effect/cpu_particles_component.h"
#include "renderer/shaders/register_locations_vertex_shader.h"
#include "renderer/shaders/register_locations_pixel_shader.h"
#include "light_system.h"
#include "application.h"
#include "scene_loader.h"
#include "render_system.h"
#include "ecs_factory.h"
#include "../utility/maths.h"

#define LINE_MAX_VERTEX_COUNT 1000
#define LINE_VERTEX_COUNT 2

TransformSystem* TransformSystem::GetSingleton()
{
	static TransformSystem singleton;
	return &singleton;
}

TransformSystem::TransformSystem()
    : System("TransformSystem", UpdateOrder::Async, true)
{
}

void TransformSystem::calculateTransforms(Scene* scene)
{
	Entity& entity = scene->getEntity();
	if (TransformComponent* transform = entity.getComponent<TransformComponent>())
	{
		int passDown = transform->getPassDowns();
		if (SpringArmComponent* springArm = entity.getComponent<SpringArmComponent>())
		{
			Vector3 parentPosition = transform->getParentAbsoluteTransform().Translation();
			Vector3 childPosition = transform->getPosition();
			Vector3 intialPosition = parentPosition + childPosition;
			Vector3 finalPosition = parentPosition + springArm->getDesiredLocalPosition();

			transform->setAbsolutePosition(Vector3::Lerp(intialPosition, finalPosition, springArm->m_Lerp));
		}
		if (passDown == (int)TransformPassDown::All)
		{
			RenderSystem::GetSingleton()->pushMatrix(transform->getLocalTransform());
		}
		else
		{
			Matrix matrix = Matrix::Identity;
			if (passDown & (int)TransformPassDown::Position)
			{
				matrix = Matrix::CreateTranslation(transform->getPosition()) * matrix;
			}
			if (passDown & (int)TransformPassDown::Rotation)
			{
				matrix = Matrix::CreateFromQuaternion(transform->getRotation()) * matrix;
			}
			if (passDown & (int)TransformPassDown::Scale)
			{
				matrix = Matrix::CreateScale(transform->getScale()) * matrix;
			}
			RenderSystem::GetSingleton()->pushMatrix(matrix);
		}
	}
	else
	{
		RenderSystem::GetSingleton()->pushMatrix(Matrix::Identity);
	}

	for (auto& child : scene->getChildren())
	{
		Entity& childEntity = child->getEntity();
		if (TransformComponent* childTransform = childEntity.getComponent<TransformComponent>())
		{
			childTransform->setParentAbsoluteTransform(RenderSystem::GetSingleton()->getCurrentMatrix());
		}

		calculateTransforms(child.get());
	}
	RenderSystem::GetSingleton()->popMatrix();
}
