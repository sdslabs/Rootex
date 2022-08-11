#include "transform_system.h"

TransformSystem* TransformSystem::GetSingleton()
{
	static TransformSystem singleton1;
	return &singleton1;
}

TransformSystem::TransformSystem()
    : System("TransformSystem", UpdateOrder::Update, true)
{
	m_TransformationStack.push_back(Matrix::Identity);
}

void TransformSystem::calculateTransforms(Scene* scene)
{
	Entity& entity = scene->getEntity();
	if (TransformComponent* transform = entity.getComponent<TransformComponent>())
	{
		int passDown = transform->getPassDowns();

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
			childTransform->setParentAbsoluteTransform(getCurrentMatrix());
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
