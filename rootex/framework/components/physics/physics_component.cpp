#include "physics_component.h"
#include "core/resource_loader.h"
#include "framework/systems/physics_system.h"

#include "entity.h"

PhysicsComponent::PhysicsComponent(const String& matName, float volume, const Ref<btCollisionShape>& collisionShape)
    : m_MaterialName(matName)
    , m_MotionState(Matrix::Identity)
	, m_Material(0, 0)
    , m_Volume(volume)
{
	m_CollisionShape = collisionShape;
	m_TransformComponent = nullptr;
	physicsMaterial = ResourceLoader::CreateLuaTextResourceFile("game/assets/config/physics.lua");
	LuaInterpreter::GetSingleton()->getLuaState().script(physicsMaterial->getString());
	sol::table materialLua = LuaInterpreter::GetSingleton()->getLuaState()["PhysicsMaterial"];
	m_SpecificGravity = float(materialLua[matName]["specificgravity"]);
	m_Material.m_Friction = float(materialLua[matName]["friction"]);
	m_Material.m_Restitution = float(materialLua[matName]["restitution"]);

	m_Mass = volume * m_SpecificGravity;
	localInertia = btVector3(0.f, 0.f, 0.f);
}

bool PhysicsComponent::setup()
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
			transform = m_TransformComponent->getAbsoluteTransform();
			m_MotionState.setWorldTransform(matTobtTransform(transform));
			btRigidBody::btRigidBodyConstructionInfo rbInfo(m_Mass, &m_MotionState, m_CollisionShape.get(), localInertia);

			// set up the materal properties
			rbInfo.m_restitution = m_Material.m_Restitution;
			rbInfo.m_friction = m_Material.m_Friction;

			m_Body = new btRigidBody(rbInfo);

			PhysicsSystem::GetSingleton()->addRigidBody(m_Body);
		}
	}

	return status;
}

PhysicsComponent::MotionState::MotionState(Matrix const& startingTransform)
    : m_WorldToPositionTransform(startingTransform)
{
}

void PhysicsComponent::MotionState::getWorldTransform(btTransform& worldTrans) const
{
	worldTrans = matTobtTransform(m_WorldToPositionTransform);
}

void PhysicsComponent::MotionState::setWorldTransform(const btTransform& worldTrans)
{
	m_WorldToPositionTransform = btTransformToMat(worldTrans);
}

/*PhysicsComponent::MaterialData::MaterialData(float restitution, float friction)
{
	m_Restitution = restitution;
	m_Friction = friction;
}*/

void PhysicsComponent::applyForce(const Vector3 force)
{
	m_Body->applyCentralImpulse(vecTobtVector3(force));
}

void PhysicsComponent::applyTorque(const Vector3 torque)
{
	m_Body->applyTorqueImpulse(vecTobtVector3(torque));
}

void PhysicsComponent::kinematicMove(const Matrix& matrix)
{
	m_Body->setActivationState(DISABLE_DEACTIVATION);
	m_Body->setWorldTransform(matTobtTransform(matrix));
}

void PhysicsComponent::setTransform(const Matrix& mat)
{
	m_Body->setActivationState(DISABLE_DEACTIVATION);
	// warp the body to the new position
	m_Body->setWorldTransform(matTobtTransform(mat));
}

Matrix PhysicsComponent::getTransform()
{
	return btTransformToMat(m_Body->getCenterOfMassTransform());
}

void PhysicsComponent::setVelocity(const Vector3& velocity)
{
	m_Body->setLinearVelocity(vecTobtVector3(velocity));
}

Vector3 PhysicsComponent::getVelocity()
{
	return btVector3ToVec(m_Body->getLinearVelocity());
}

void PhysicsComponent::setAngularVelocity(const Vector3& angularVel)
{
	m_Body->setAngularVelocity(vecTobtVector3(angularVel));
}

Vector3 PhysicsComponent::getAngularVelocity()
{
	return btVector3ToVec(m_Body->getAngularVelocity());
}

void PhysicsComponent::translate(const Vector3& vec)
{
	m_Body->translate(vecTobtVector3(vec));
}

btTransform PhysicsComponent::matTobtTransform(Matrix const& mat)
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

Matrix PhysicsComponent::btTransformToMat(btTransform const& trans)
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

void PhysicsComponent::disableGravity()
{
	m_Body->setGravity({ 0.0f, 0.0f, 0.0f });	
}

btVector3 PhysicsComponent::vecTobtVector3(Vector3 const& vec3)
{
	return btVector3(vec3.x, vec3.y, vec3.z);
}

Vector3 PhysicsComponent::btVector3ToVec(btVector3 const& btvec)
{
	return Vector3(btvec.x(), btvec.y(), btvec.z());
}
