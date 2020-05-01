#include "physics_collider_component.h"

#include "framework/systems/physics_system.h"
#include "framework/components/script_component.h"

#include "entity.h"

PhysicsColliderComponent::PhysicsColliderComponent(const String& matName, float volume, const Vector3& gravity, bool isMoveable, const Ref<btCollisionShape>& collisionShape, bool generatesHitEvents)
    : m_MaterialName(matName)
	, m_Material(0, 0)
    , m_Volume(volume)
    , m_Gravity(gravity)
    , m_IsMoveable(isMoveable)
    , m_IsGeneratesHitEvents(generatesHitEvents)
{
	m_CollisionShape = collisionShape;
	m_TransformComponent = nullptr;
	m_ScriptComponent = nullptr;
	sol::table materialLua = PhysicsSystem::GetSingleton()->getPhysicsMaterial();
	m_SpecificGravity = float(materialLua[matName]["specificgravity"]);
	m_Material.m_Friction = float(materialLua[matName]["friction"]);
	m_Material.m_Restitution = float(materialLua[matName]["restitution"]);

	if (m_IsMoveable)
	{
		m_Mass = volume * m_SpecificGravity;
	}
	else
	{
		m_Mass = 0;
	}

	m_RenderColor = ColorPresets::LightCoral;

	m_LocalInertia = btVector3(0.0f, 0.0f, 0.0f);
}

bool PhysicsColliderComponent::setup()
{
	bool status = true;
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
		if (!m_TransformComponent)
		{
			ERR("TransformComponent not found on entity with PhysicsComponent: " + m_Owner->getFullName());
			status = false;
		}
		else
		{
			btRigidBody::btRigidBodyConstructionInfo rbInfo(m_Mass, this, m_CollisionShape.get(), m_LocalInertia);

			/// Set up the materal properties.
			rbInfo.m_restitution = m_Material.m_Restitution;
			rbInfo.m_friction = m_Material.m_Friction;

			m_Body = new btRigidBody(rbInfo);

			/// Adds a new rigid body to physics system.
			PhysicsSystem::GetSingleton()->addRigidBody(m_Body);
			setGravity(m_Gravity);
			setMoveable(m_IsMoveable);
			
			m_Body->setUserPointer(this);
			
			m_ScriptComponent = getOwner()->getComponent<ScriptComponent>().get();
			if (m_IsGeneratesHitEvents && !m_ScriptComponent)
			{
				WARN("ScriptComponent not found on entity with a PhysicsComponent that generates hit events: " + m_Owner->getFullName());
			}
		}
	}

	return status;
}

void PhysicsColliderComponent::getWorldTransform(btTransform& worldTrans) const
{
	worldTrans = matTobtTransform(m_TransformComponent->getRotationPosition());
}

void PhysicsColliderComponent::setWorldTransform(const btTransform& worldTrans)
{
	m_TransformComponent->setRotationPosition(btTransformToMat(worldTrans));
}

void PhysicsColliderComponent::applyForce(const Vector3& force)
{
	m_Body->applyCentralImpulse(vecTobtVector3(force));
}

void PhysicsColliderComponent::applyTorque(const Vector3& torque)
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

void PhysicsColliderComponent::setMoveable(bool enabled)
{
	if (enabled)
	{
		m_Mass = m_Volume * m_SpecificGravity;
	}
	else
	{
		m_Mass = 0.0f;
	}
	m_Body->setMassProps(m_Mass, m_LocalInertia);
}

void PhysicsColliderComponent::render()
{
	PhysicsSystem::GetSingleton()->debugDrawComponent(matTobtTransform(m_TransformComponent->getRotationPosition()), m_CollisionShape.get(), { m_RenderColor.x, m_RenderColor.y, m_RenderColor.z });
}

JSON::json PhysicsColliderComponent::getJSON() const
{
	JSON::json j;

	j["gravity"]["x"] = m_Gravity.x;
	j["gravity"]["y"] = m_Gravity.y;
	j["gravity"]["z"] = m_Gravity.z;
	
	j["isMoveable"] = m_IsMoveable;
	j["isGeneratesHitEvents"] = m_IsGeneratesHitEvents;

	return j;
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
			// this is because Mat4x4s are row-major matrices and
			// btMatrix3x3 are column-major.  This reversed indexing
			// implicitly transposes (flips along the diagonal)
			// the matrix when it is copied.
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
			// this is because Mat4x4s are row-major matrices and
			// btMatrix3x3 are column-major.  This reversed indexing
			// implicitly transposes (flips along the diagonal)
			// the matrix when it is copied.
		}
	}

	// copy position
	for (int column = 0; column < 3; ++column)
	{
		returnValue.m[3][column] = bulletPosition[column];
	}

	return returnValue;
}

void PhysicsColliderComponent::setGravity(const Vector3& gravity)
{
	m_Body->setGravity(vecTobtVector3(gravity));
}

btVector3 PhysicsColliderComponent::vecTobtVector3(Vector3 const& vec3)
{
	return btVector3(vec3.x, vec3.y, vec3.z);
}

Vector3 PhysicsColliderComponent::btVector3ToVec(btVector3 const& btvec)
{
	return Vector3(btvec.x(), btvec.y(), btvec.z());
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void PhysicsColliderComponent::draw()
{
	if (ImGui::BeginCombo("Material", m_MaterialName.c_str()))
	{
		for (auto&& material : PhysicsSystem::GetSingleton()->getPhysicsMaterial())
		{
			if (ImGui::Selectable(material.first.as<String>().c_str()))
			{
				m_MaterialName.assign(material.first.as<String>().c_str());
			}
		}
		ImGui::EndCombo();
	}
	
	if (ImGui::Checkbox("Moveable", &m_IsMoveable))
	{
		setMoveable(m_IsMoveable);
	}

	if (ImGui::Checkbox("Generates Hit Events", &m_IsGeneratesHitEvents))
	{
		if (m_IsGeneratesHitEvents)
		{
			setup();
		}
	}

	if (ImGui::DragFloat3("Gravity", &m_Gravity.x))
	{
		setGravity(m_Gravity);
	}
	
	ImGui::ColorEdit3("Render Color", &m_RenderColor.x);
}
#endif // ROOTEX_EDITOR
