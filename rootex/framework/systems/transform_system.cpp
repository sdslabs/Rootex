#include "transform_system.h"

#include "components/game/spring_arm_component.h"

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
	m_TransformationStack.push_back(Matrix::Identity);
}

void TransformSystem::calculateTransforms(Scene* scene)
{
	Entity& entity = scene->getEntity();
	if (TransformComponent* transform = entity.getComponent<TransformComponent>())
	{
		int passDown = transform->getPassDowns();
		if (SpringArmComponent* springArm = entity.getComponent<SpringArmComponent>())
		{
			Vector3 parentAbsolutePosition = entity.getScene()->getParent()->getEntity().getComponent<TransformComponent>()->getAbsolutePosition();
			Vector3 initialPosition = transform->getAbsolutePosition() - parentAbsolutePosition;
			Vector3 finalPosition = springArm->getDesiredLocalPosition();

			Vector3 diff = finalPosition - initialPosition;

			if (springArm->getLerp() > 0.05 && (std::abs(diff.x) > 0.01 || std::abs(diff.y) > 0.01 || std::abs(diff.z) > 0.01))
			{
				transform->setPosition(parentAbsolutePosition + Vector3::Lerp(initialPosition, finalPosition, springArm->getLerp()));
			}
		}
		if (passDown == (int)TransformPassDown::All)
		{
			pushMatrix(transform->getLocalTransform());
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
			pushMatrix(matrix);
		}
	}
	else
	{
		pushMatrix(Matrix::Identity);
	}

	for (auto& child : scene->getChildren())
	{
		Entity& childEntity = child->getEntity();
		if (TransformComponent* childTransform = childEntity.getComponent<TransformComponent>())
		{
			if (childEntity.getComponent<SpringArmComponent>())
			{
				childTransform->setParentAbsoluteTransform(Matrix::Identity);
			}
			else
			{
				childTransform->setParentAbsoluteTransform(getCurrentMatrix());
			}
		}

		calculateTransforms(child.get());
	}
	popMatrix();
}

void TransformSystem::pushMatrix(const Matrix& transform)
{
	m_TransformationStack.push_back(transform * m_TransformationStack.back());
}

void TransformSystem::pushMatrixOverride(const Matrix& transform)
{
	m_TransformationStack.push_back(transform);
}

void TransformSystem::popMatrix()
{
	m_TransformationStack.pop_back();
}

const Matrix& TransformSystem::getCurrentMatrix() const
{
	return m_TransformationStack.back();
}
