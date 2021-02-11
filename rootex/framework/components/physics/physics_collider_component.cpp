#include "physics_collider_component.h"

#include "framework/systems/physics_system.h"
#include "script/script.h"

#include "entity.h"

btTransform MatTobtTransform(Matrix const& mat)
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

Matrix BtTransformToMat(btTransform const& trans)
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

btVector3 VecTobtVector3(Vector3 const& vec3)
{
	return btVector3(vec3.x, vec3.y, vec3.z);
}

Vector3 BtVector3ToVec(btVector3 const& btvec)
{
	return Vector3(btvec.x(), btvec.y(), btvec.z());
}

void PhysicsColliderComponent::RegisterAPI(sol::table& rootex)
{
	sol::usertype<PhysicsColliderComponent> physicsColliderComponent = rootex.new_usertype<PhysicsColliderComponent>(
	    "PhysicsColliderComponent",
	    sol::base_classes, sol::bases<Component>());
	rootex["Entity"]["getPhysicsCollider"] = &Entity::getComponent<PhysicsColliderComponent>;
	physicsColliderComponent["getVelocity"] = &PhysicsColliderComponent::getVelocity;
	physicsColliderComponent["setVelocity"] = &PhysicsColliderComponent::setVelocity;
	physicsColliderComponent["applyForce"] = &PhysicsColliderComponent::applyForce;
}

PhysicsColliderComponent::PhysicsColliderComponent(const PhysicsMaterial& material, float volume, const Vector3& gravity, const Vector3& angularFactor, int collisionGroup, int collisionMask, bool isMoveable, bool isKinematic, bool generatesHitEvents, const Ref<btCollisionShape>& collisionShape)
    : m_Material(material)
    , m_Volume(volume)
    , m_Gravity(gravity)
    , m_AngularFactor(angularFactor)
    , m_CollisionGroup(collisionGroup)
    , m_CollisionMask(collisionMask)
    , m_IsMoveable(isMoveable)
    , m_IsGeneratesHitEvents(generatesHitEvents)
    , m_IsKinematic(isKinematic)
    , m_DependencyOnTransformComponent(this)
{
	m_CollisionShape = collisionShape;

	if (m_IsMoveable)
	{
		m_Mass = volume * PhysicsSystem::GetSingleton()->getMaterialData(m_Material).specificGravity;
	}
	else
	{
		m_Mass = 0.0f;
	}

	if (m_Mass > 0.0f)
	{
		m_CollisionShape->calculateLocalInertia(m_Mass, m_LocalInertia);
	}
}

bool PhysicsColliderComponent::setupData()
{
	if (m_Body)
	{
		PhysicsSystem::GetSingleton()->removeRigidBody(m_Body.get());
	}

	btRigidBody::btRigidBodyConstructionInfo rbInfo(m_Mass, this, m_CollisionShape.get(), m_LocalInertia);
	{
		const PhysicsMaterialData& materialData = PhysicsSystem::GetSingleton()->getMaterialData(m_Material);
		rbInfo.m_restitution = materialData.restitution;
		rbInfo.m_friction = materialData.friction;
	}
	m_Body.reset(new btRigidBody(rbInfo));

	PhysicsSystem::GetSingleton()->addRigidBody(m_Body.get(), m_CollisionGroup, m_CollisionMask);
	setGravity(m_Gravity);
	setMoveable(m_IsMoveable);
	setKinematic(m_IsKinematic);
	setAngularFactor(m_AngularFactor);
	m_Body->setUserPointer(this);

	return true;
}

void PhysicsColliderComponent::onRemove()
{
	PhysicsSystem::GetSingleton()->removeRigidBody(m_Body.get());
}

void PhysicsColliderComponent::getWorldTransform(btTransform& worldTrans) const
{
	worldTrans = MatTobtTransform(m_TransformComponent->getRotationPosition());
}

void PhysicsColliderComponent::setWorldTransform(const btTransform& worldTrans)
{
	m_Body->setActivationState(DISABLE_DEACTIVATION);
	m_TransformComponent->setRotationPosition(BtTransformToMat(worldTrans));
}

void PhysicsColliderComponent::applyForce(const Vector3& force)
{
	m_Body->applyCentralImpulse(VecTobtVector3(force));
}

void PhysicsColliderComponent::applyTorque(const Vector3& torque)
{
	m_Body->applyTorqueImpulse(VecTobtVector3(torque));
}

void PhysicsColliderComponent::setAngularFactor(const Vector3& factors)
{
	m_AngularFactor = factors;
	m_Body->setAngularFactor(VecTobtVector3(factors));
}

void PhysicsColliderComponent::setAxisLock(bool enabled)
{
	if (enabled)
	{
		setAngularFactor({ 0.0f, 0.0f, 0.0f });
	}
	else
	{
		setAngularFactor({ 1.0f, 1.0f, 1.0f });
	}
}

void PhysicsColliderComponent::setTransform(const Matrix& mat)
{
	m_Body->setActivationState(DISABLE_DEACTIVATION);
	// warp the body to the new position
	m_Body->setWorldTransform(MatTobtTransform(mat));
}

Matrix PhysicsColliderComponent::getTransform()
{
	return BtTransformToMat(m_Body->getCenterOfMassTransform());
}

void PhysicsColliderComponent::setMoveable(bool enabled)
{
	m_IsMoveable = enabled;
	if (enabled)
	{
		m_Mass = m_Volume * PhysicsSystem::GetSingleton()->getMaterialData(m_Material).specificGravity;
		m_Body->setCollisionFlags(m_Body->getCollisionFlags() ^ btCollisionObject::CF_STATIC_OBJECT);
	}
	else
	{
		m_Mass = 0.0f;
		m_Body->setCollisionFlags(m_Body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	}
	m_Body->setMassProps(m_Mass, m_LocalInertia);
}

void PhysicsColliderComponent::setKinematic(bool enabled)
{
	m_IsKinematic = enabled;
	if (enabled)
	{
		m_Body->forceActivationState(DISABLE_DEACTIVATION);
		m_Body->setCollisionFlags(m_Body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}
	else
	{
		m_Body->setActivationState(ACTIVE_TAG);
	}
}

void PhysicsColliderComponent::highlight()
{
	PhysicsSystem::GetSingleton()->debugDrawComponent(MatTobtTransform(m_TransformComponent->getRotationPosition()), m_CollisionShape.get(), VecTobtVector3({ 0.0f, 1.0f, 0.0f }));
}

JSON::json PhysicsColliderComponent::getJSON() const
{
	JSON::json j;

	j["angularFactor"] = m_AngularFactor;
	j["gravity"] = m_Gravity;
	j["collisionGroup"] = m_CollisionGroup;
	j["collisionMask"] = m_CollisionMask;
	j["volume"] = m_Volume;
	j["material"] = (int)m_Material;
	j["isMoveable"] = m_IsMoveable;
	j["isKinematic"] = m_IsKinematic;
	j["isGeneratesHitEvents"] = m_IsGeneratesHitEvents;

	return j;
}

void PhysicsColliderComponent::setVelocity(const Vector3& velocity)
{
	m_Body->setLinearVelocity(VecTobtVector3(velocity));
}

Vector3 PhysicsColliderComponent::getVelocity()
{
	return BtVector3ToVec(m_Body->getLinearVelocity());
}

void PhysicsColliderComponent::setAngularVelocity(const Vector3& angularVel)
{
	m_Body->setAngularVelocity(VecTobtVector3(angularVel));
}

Vector3 PhysicsColliderComponent::getAngularVelocity()
{
	return BtVector3ToVec(m_Body->getAngularVelocity());
}

void PhysicsColliderComponent::translate(const Vector3& vec)
{
	m_Body->translate(VecTobtVector3(vec));
}

void PhysicsColliderComponent::setGravity(const Vector3& gravity)
{
	m_Body->setGravity(VecTobtVector3(gravity));
}

PhysicsMaterial PhysicsColliderComponent::getMaterial() const
{
	return m_Material;
}

void PhysicsColliderComponent::draw()
{
	static bool showInEditor = true;
	ImGui::Checkbox("Show in Editor", &showInEditor);
	if (showInEditor)
	{
		highlight();
	}

	ImGui::Combo("Physics Material", (int*)&m_Material, PhysicsSystem::GetSingleton()->getMaterialNames());

	if (ImGui::DragFloat3("Gravity", &m_Gravity.x))
	{
		setGravity(m_Gravity);
	}

	if (ImGui::DragFloat3("Angular Factor", &m_AngularFactor.x))
	{
		setAngularFactor(m_AngularFactor);
	}

	if (ImGui::Button("Apply Axis Lock"))
	{
		setAxisLock(true);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Axis Lock"))
	{
		setAxisLock(false);
	}

	if (ImGui::Checkbox("Moveable", &m_IsMoveable))
	{
		setMoveable(m_IsMoveable);
	}
	
	if (ImGui::Checkbox("Kinematic", &m_IsKinematic))
	{
		setKinematic(m_IsKinematic);
	}

	if (ImGui::Checkbox("Generates Hit Events", &m_IsGeneratesHitEvents))
	{
		if (m_IsGeneratesHitEvents)
		{
			setupData();
		}
	}

	if (ImGui::TreeNodeEx("Collision Group"))
	{
		displayCollisionLayers(m_CollisionGroup);
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Collision Mask"))
	{
		displayCollisionLayers(m_CollisionMask);
		ImGui::TreePop();
	}
}

void PhysicsColliderComponent::displayCollisionLayers(unsigned int& collision)
{
	ImGui::CheckboxFlags("Player", &collision, (int)CollisionMask::Player);
	ImGui::CheckboxFlags("Enemy", &collision, (int)CollisionMask::Enemy);
	ImGui::CheckboxFlags("Architecture", &collision, (int)CollisionMask::Architecture);
	ImGui::CheckboxFlags("TriggerVolume", &collision, (int)CollisionMask::TriggerVolume);
	ImGui::CheckboxFlags("All", &collision, (int)CollisionMask::All);
}
