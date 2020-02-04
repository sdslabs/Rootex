#include "physics_component.h"
#include "core/resource_loader.h"
#include "physics/physics.h"

#include "entity.h"

PhysicsComponent::PhysicsComponent(btScalar mass, std::string& matName)
    : mass(mass)
    , m_MaterialName(matName)
    , m_Material(lookUpMaterialData(m_MaterialName))
    , m_MotionState(Matrix::Identity)
{
	m_TransformComponent = nullptr;
	physicsMaterial = ResourceLoader::CreateLuaTextResourceFile("game/assets/config/physics.lua");
	physicsLua.loadExecuteScript(physicsMaterial);
	LuaVariable materialLua = physicsLua.getGlobal("m_MaterialName");

	localInertia = btVector3(0.f, 0.f, 0.f);
	if (mass > 0.f)
	{
		collisionShape->calculateLocalInertia(mass, localInertia);
	}
}

bool PhysicsComponent::setup()
{
	bool status = true;
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>();
		if (m_TransformComponent == nullptr)
		{
			status = false;
		}
		else
		{
			transform = m_TransformComponent->getTransform();

			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, &m_MotionState, collisionShape.get(), localInertia);

			// set up the materal properties
			rbInfo.m_restitution = m_Material.m_Restitution;
			rbInfo.m_friction = m_Material.m_Friction;

			body = new btRigidBody(rbInfo);

			GamePhysics::GetSingleton()->addRigidBody(body);
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

PhysicsComponent::MaterialData::MaterialData(float restitution, float friction)
{
	m_Restitution = restitution;
	m_Friction = friction;
}

float PhysicsComponent::lookUpSpecificGravity(const std::string& matName)
{
}

PhysicsComponent::MaterialData PhysicsComponent::lookUpMaterialData(const std::string& materialName)
{
}

void PhysicsComponent::applyForce(const Vector3 force)
{
	body->applyCentralImpulse(vecTobtVector3(force));
}

void PhysicsComponent::applyTorque(const Vector3 torque)
{
	body->applyTorqueImpulse(vecTobtVector3(torque));
}

void PhysicsComponent::setVelocity(const Vector3& velocity)
{
	body->setLinearVelocity(vecTobtVector3(velocity));
}

Vector3 PhysicsComponent::getVelocity()
{
	return btVector3ToVec(body->getLinearVelocity());
}

void PhysicsComponent::setAngularVelocity(const Vector3& angularVel)
{
	body->setAngularVelocity(vecTobtVector3(angularVel));
}

Vector3 PhysicsComponent::getAngularVelocity()
{
	return btVector3ToVec(body->getAngularVelocity());
}

void PhysicsComponent::translate(const Vector3& vec)
{
	body->translate(vecTobtVector3(vec));
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

btVector3 PhysicsComponent::vecTobtVector3(Vector3 const& vec3)
{
	return btVector3(vec3.x, vec3.y, vec3.z);
}

Vector3 PhysicsComponent::btVector3ToVec(btVector3 const& btvec)
{
	return Vector3(btvec.x(), btvec.y(), btvec.z());
}
