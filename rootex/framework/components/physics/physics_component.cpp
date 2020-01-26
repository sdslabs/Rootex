#include "physics_component.h"

#include "entity.h"

Component* PhysicsComponent::Create(const LuaVariable& componentData)
{
	PhysicsComponent* component = new PhysicsComponent();

	return component;
}

PhysicsComponent::MaterialData::MaterialData(float restitution, float friction)
{
	m_Restitution = restitution;
	m_Friction = friction;
}

btTransform PhysicsComponent::Mat4x4_to_btTransform(Matrix const& mat)
{
	// convert from Mat4x4 to btTransform
	btMatrix3x3 bulletRotation;
	btVector3 bulletPosition;

	// copy rotation matrix
	for (int row = 0; row < 3; ++row)
	{
		for (int column = 0; column < 3; ++column)
		{
			bulletRotation[row][column] = mat.m[column][row];
			// note the reversed indexing (row/column vs. column/row)
			//  this is because Mat4x4s are row-major matrices and
			//  btMatrix3x3 are column-major.  This reversed indexing
			//  implicitly transposes (flips along the diagonal)
			//  the matrix when it is copied.
		}
	}

	// copy position
	for (int column = 0; column < 3; ++column)
	{
		bulletPosition[column] = mat.m[3][column];
	}

	return btTransform(bulletRotation, bulletPosition);
}