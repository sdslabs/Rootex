#include "rigid_body_component.h"

#include "common/types.h"

#include "framework/systems/physics_system.h"
#include "script/script.h"

#include "entity.h"

RigidBodyComponent::RigidBodyComponent(
    const PhysicsMaterial& material,
    float volume,
    const Vector3& offset,
    const Vector3& gravity,
    const Vector3& angularFactor,
    int collisionGroup,
    int collisionMask,
    bool isMoveable,
    bool isKinematic,
    bool generatesHitEvents,
    bool isSleepable,
    bool isCCD,
    const Ref<btCollisionShape>& collisionShape)
    : CollisionComponent(collisionGroup, collisionMask)
	, m_Material(material)
    , m_Volume(volume)
    , m_Offset(offset)
    , m_Gravity(gravity)
    , m_AngularFactor(angularFactor)
    , m_IsMoveable(isMoveable)
    , m_IsGeneratesHitEvents(generatesHitEvents)
    , m_IsSleepable(isSleepable)
    , m_IsKinematic(isKinematic)
    , m_IsCCD(isCCD)
    , m_DependencyOnTransformComponent(this)
{
	m_CollisionShape = collisionShape;
}

bool RigidBodyComponent::setupData()
{
	if (m_Body)
	{
		PhysicsSystem::GetSingleton()->removeRigidBody(m_Body.get());
	}

	if (m_IsMoveable)
	{
		m_Mass = m_Volume * PhysicsSystem::GetSingleton()->getMaterialData(m_Material).specificGravity;
		m_CollisionShape->calculateLocalInertia(m_Mass, m_LocalInertia);
	}
	else
	{
		m_Mass = 0.0f;
	}

	btRigidBody::btRigidBodyConstructionInfo rbInfo(m_Mass, this, m_CollisionShape.get(), m_LocalInertia);
	{
		const PhysicsMaterialData& materialData = PhysicsSystem::GetSingleton()->getMaterialData(m_Material);
		rbInfo.m_restitution = materialData.restitution;
		rbInfo.m_friction = materialData.friction;
	}
	m_Body.reset(new btRigidBody(rbInfo));

	setGravity(m_Gravity);
	setMoveable(m_IsMoveable);
	setKinematic(m_IsKinematic);
	setAngularFactor(m_AngularFactor);
	setSleepable(m_IsSleepable);
	setCCD(m_IsCCD);
	m_Body->setUserPointer((CollisionComponent*)this);

	PhysicsSystem::GetSingleton()->addRigidBody(m_Body.get(), m_CollisionGroup, m_CollisionMask);
	return true;
}

void RigidBodyComponent::onRemove()
{
	if (m_Body)
	{
		PhysicsSystem::GetSingleton()->removeRigidBody(m_Body.get());
	}
}

void RigidBodyComponent::getWorldTransform(btTransform& worldTrans) const
{
	worldTrans = MatTobtTransform(m_TransformComponent->getRotationPosition() * Matrix::CreateTranslation(m_Offset));
}

void RigidBodyComponent::setWorldTransform(const btTransform& worldTrans)
{
	m_TransformComponent->setAbsoluteRotationPosition(Matrix::CreateTranslation(-m_Offset) * BtTransformToMat(worldTrans));
}

void RigidBodyComponent::handleHit(Hit* h)
{
	if (m_IsGeneratesHitEvents)
	{
		m_Owner->call("hit", { h });
	}
}

void RigidBodyComponent::applyForce(const Vector3& force)
{
	m_Body->activate(true);
	m_Body->applyCentralImpulse(VecTobtVector3(force));
}

void RigidBodyComponent::applyTorque(const Vector3& torque)
{
	m_Body->activate(true);
	m_Body->applyTorqueImpulse(VecTobtVector3(torque));
}

void RigidBodyComponent::setAngularFactor(const Vector3& factors)
{
	m_AngularFactor = factors;
	m_Body->activate(true);
	m_Body->setAngularFactor(VecTobtVector3(factors));
}

void RigidBodyComponent::setAxisLock(bool enabled)
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

void RigidBodyComponent::setOffset(const Vector3& offset)
{
	m_Offset = offset;
	m_Body->activate(true);
	setupData();
}

void RigidBodyComponent::setTransform(const Matrix& mat)
{
	m_Body->activate(true);
	m_Body->setWorldTransform(MatTobtTransform(mat));
}

Matrix RigidBodyComponent::getTransform()
{
	return BtTransformToMat(m_Body->getCenterOfMassTransform());
}

void RigidBodyComponent::setMoveable(bool enabled)
{
	m_IsMoveable = enabled;
	if (enabled)
	{
		m_Mass = m_Volume * PhysicsSystem::GetSingleton()->getMaterialData(m_Material).specificGravity;
		m_Body->activate(true);
		m_Body->setCollisionFlags(m_Body->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
	}
	else
	{
		m_Mass = 0.0f;
		m_Body->activate(true);
		m_Body->setCollisionFlags(m_Body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	}
	m_Body->setMassProps(m_Mass, m_LocalInertia);
}

void RigidBodyComponent::setSleepable(bool enabled)
{
	m_IsSleepable = enabled;
	if (enabled)
	{
		m_Body->forceActivationState(ACTIVE_TAG);
	}
	else
	{
		m_Body->forceActivationState(DISABLE_DEACTIVATION);
	}
}

void RigidBodyComponent::setCCD(bool enabled)
{
	// https://github.com/godotengine/godot/blob/46de553473b4bea49176fb4316176a5662931160/modules/bullet/rigid_body_bullet.cpp#L722

	if (enabled)
	{
		// This threshold enable CCD if the object moves more than
		// 1 meter in one simulation frame
		m_Body->setCcdMotionThreshold(1e-7f);

		/// Calculate using the rule write below the CCD swept sphere radius
		///     CCD works on an embedded sphere of radius, make sure this radius
		///     is embedded inside the convex objects, preferably smaller:
		///     for an object of dimensions 1 metre, try 0.2
		btScalar radius(1.0f);
		if (m_Body->getCollisionShape())
		{
			btVector3 center;
			m_Body->getCollisionShape()->getBoundingSphere(center, radius);
		}
		m_Body->setCcdSweptSphereRadius(radius * 0.2f);
	}
	else
	{
		m_Body->setCcdMotionThreshold(0.0f);
		m_Body->setCcdSweptSphereRadius(0.0f);
	}
}

void RigidBodyComponent::setKinematic(bool enabled)
{
	m_IsKinematic = enabled;
	if (enabled)
	{
		m_Body->forceActivationState(DISABLE_DEACTIVATION);
		m_Body->setCollisionFlags(m_Body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}
	else
	{
		m_Body->activate(true);
		m_Body->setCollisionFlags(m_Body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
	}
}

void RigidBodyComponent::highlight()
{
	PhysicsSystem::GetSingleton()->debugDrawComponent(
	    MatTobtTransform(m_TransformComponent->getRotationPosition() * Matrix::CreateTranslation(m_Offset)),
	    m_CollisionShape.get(),
	    VecTobtVector3({ 0.8f, 0.1f, 0.1f }));
}

JSON::json RigidBodyComponent::getJSON() const
{
	JSON::json j = CollisionComponent::getJSON();

	j["angularFactor"] = m_AngularFactor;
	j["offset"] = m_Offset;
	j["gravity"] = m_Gravity;
	j["volume"] = m_Volume;
	j["material"] = (int)m_Material;
	j["isMoveable"] = m_IsMoveable;
	j["isKinematic"] = m_IsKinematic;
	j["isSleepable"] = m_IsSleepable;
	j["isCCD"] = m_IsSleepable;
	j["isGeneratesHitEvents"] = m_IsGeneratesHitEvents;

	return j;
}

void RigidBodyComponent::setVelocity(const Vector3& velocity)
{
	m_Body->activate(true);
	m_Body->setLinearVelocity(VecTobtVector3(velocity));
}

Vector3 RigidBodyComponent::getVelocity()
{
	return BtVector3ToVec(m_Body->getLinearVelocity());
}

void RigidBodyComponent::setAngularVelocity(const Vector3& angularVel)
{
	m_Body->activate(true);
	m_Body->setAngularVelocity(VecTobtVector3(angularVel));
}

Vector3 RigidBodyComponent::getAngularVelocity()
{
	return BtVector3ToVec(m_Body->getAngularVelocity());
}

void RigidBodyComponent::translate(const Vector3& vec)
{
	m_Body->activate(true);
	m_Body->translate(VecTobtVector3(vec));
}

void RigidBodyComponent::setGravity(const Vector3& gravity)
{
	m_Body->activate(true);
	m_Body->setGravity(VecTobtVector3(gravity));
}

PhysicsMaterial RigidBodyComponent::getMaterial() const
{
	return m_Material;
}

void RigidBodyComponent::draw()
{
	static bool showInEditor = true;
	ImGui::Checkbox("Show in Editor", &showInEditor);
	if (showInEditor)
	{
		highlight();
	}

	ImGui::Combo("Physics Material", (int*)&m_Material, PhysicsSystem::GetSingleton()->getMaterialNames());

	if (ImGui::DragFloat3("##Offset", &m_Offset.x, 0.01f))
	{
		setOffset(m_Offset);
	}
	ImGui::SameLine();
	if (ImGui::Button("Offset"))
	{
		setOffset({ 0.0f, 0.0f, 0.0f });
	}

	if (ImGui::DragFloat3("Gravity", &m_Gravity.x, 0.01f))
	{
		setGravity(m_Gravity);
	}

	if (ImGui::DragFloat3("Angular Factor", &m_AngularFactor.x, 0.01f))
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

	if (ImGui::Checkbox("Sleepable", &m_IsSleepable))
	{
		setSleepable(m_IsSleepable);
	}

	ImGui::Checkbox("Generates Hit Events", &m_IsGeneratesHitEvents);

	if (ImGui::Checkbox("CCD", &m_IsCCD))
	{
		setCCD(m_IsCCD);
	}

	CollisionComponent::draw();
}
