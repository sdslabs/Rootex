#include "physics_collider_component.h"
#include "framework/systems/physics_system.h"

#include "entity.h"

PhysicsColliderComponent::PhysicsColliderComponent(const String& matName, float volume, const Ref<btCollisionShape>& collisionShape)
    : m_MaterialName(matName)
    , m_MotionState(Matrix::Identity)
	, m_Material(0, 0)
    , m_Volume(volume)
{
	m_CollisionShape = collisionShape;
	m_TransformComponent = nullptr;
	sol::table materialLua = PhysicsSystem::GetSingleton()->getPhysicsMaterial();
	m_SpecificGravity = float(materialLua[matName]["specificgravity"]);
	m_Material.m_Friction = float(materialLua[matName]["friction"]);
	m_Material.m_Restitution = float(materialLua[matName]["restitution"]);

	m_Mass = volume * m_SpecificGravity;
	m_LocalInertia = btVector3(0.f, 0.f, 0.f);
}

bool PhysicsColliderComponent::setup()
{
	bool status = true;
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>();
		if (!m_TransformComponent)
		{
			status = false;
		}
		else
		{
			m_Transform = m_TransformComponent->getAbsoluteTransform();
			m_MotionState.setWorldTransform(matTobtTransform(m_Transform));
			btRigidBody::btRigidBodyConstructionInfo rbInfo(m_Mass, &m_MotionState, m_CollisionShape.get(), m_LocalInertia);

			// set up the materal properties
			rbInfo.m_restitution = m_Material.m_Restitution;
			rbInfo.m_friction = m_Material.m_Friction;

			m_Body = new btRigidBody(rbInfo);

			PhysicsSystem::GetSingleton()->addRigidBody(m_Body);
		}
	}

	return status;
}

PhysicsColliderComponent::MotionState::MotionState(Matrix const& startingTransform)
    : m_WorldToPositionTransform(startingTransform)
{
}

void PhysicsColliderComponent::MotionState::getWorldTransform(btTransform& worldTrans) const
{
	worldTrans = matTobtTransform(m_WorldToPositionTransform);
}

void PhysicsColliderComponent::MotionState::setWorldTransform(const btTransform& worldTrans)
{
	m_WorldToPositionTransform = btTransformToMat(worldTrans);
}

/*PhysicsColliderComponent::MaterialData::MaterialData(float restitution, float friction)
{
	m_Restitution = restitution;
	m_Friction = friction;
}*/

void PhysicsColliderComponent::applyForce(const Vector3 force)
{
	m_Body->applyCentralImpulse(vecTobtVector3(force));
}

void PhysicsColliderComponent::applyTorque(const Vector3 torque)
{
	m_Body->applyTorqueImpulse(vecTobtVector3(torque));
}

void PhysicsColliderComponent::kinematicMove(const Matrix& matrix)
{
	m_Body->setActivationState(DISABLE_DEACTIVATION);
	m_Body->setWorldTransform(matTobtTransform(matrix));
}

void PhysicsColliderComponent::setTransform(const Matrix& mat)
{
	m_Body->setActivationState(DISABLE_DEACTIVATION);
	// warp the body to the new position
	m_Body->setWorldTransform(matTobtTransform(mat));
}

Matrix PhysicsColliderComponent::getTransform()
{
	return btTransformToMat(m_Body->getCenterOfMassTransform());
}

void PhysicsColliderComponent::setVelocity(const Vector3& velocity)
{
	m_Body->setLinearVelocity(vecTobtVector3(velocity));
}

Vector3 PhysicsColliderComponent::getVelocity()
{
	return btVector3ToVec(m_Body->getLinearVelocity());
}

void PhysicsColliderComponent::setAngularVelocity(const Vector3& angularVel)
{
	m_Body->setAngularVelocity(vecTobtVector3(angularVel));
}

Vector3 PhysicsColliderComponent::getAngularVelocity()
{
	return btVector3ToVec(m_Body->getAngularVelocity());
}

void PhysicsColliderComponent::translate(const Vector3& vec)
{
	m_Body->translate(vecTobtVector3(vec));
}

btTransform PhysicsColliderComponent::matTobtTransform(Matrix const& mat)
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

Matrix PhysicsColliderComponent::btTransformToMat(btTransform const& trans)
{
	Matrix returnValue = Matrix::Identity;

	// convert from btTransform to Mat4x4
	btMatrix3x3 const& bulletRotation = trans.getBasis();
	btVector3 const& bulletPosition = trans.getOrigin();

	// copy rotation matrix
	for (int row = 0; row < 3; ++row)
	{
		for (int column = 0; column < 3; ++column)
		{
			returnValue.m[row][column] = bulletRotation[column][row];
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
		returnValue.m[3][column] = bulletPosition[column];
	}

	return returnValue;
}

void PhysicsColliderComponent::disableGravity()
{
	m_Body->setGravity({ 0.0f, 0.0f, 0.0f });	
}

btVector3 PhysicsColliderComponent::vecTobtVector3(Vector3 const& vec3)
{
	return btVector3(vec3.x, vec3.y, vec3.z);
}

Vector3 PhysicsColliderComponent::btVector3ToVec(btVector3 const& btvec)
{
	return Vector3(btvec.x(), btvec.y(), btvec.z());
}
