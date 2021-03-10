#pragma once

#include "component.h"
#include "components/transform_component.h"
#include "core/physics/bullet_conversions.h"

#include "btBulletDynamicsCommon.h"

enum PhysicsMaterial;

enum class CollisionMask : unsigned int
{
	None = 0,
	Player = 1 << 0,
	Enemy = 1 << 1,
	Architecture = 1 << 2,
	TriggerVolume = 1 << 3,
	Other = 1 << 4,
	All = Player | Enemy | Architecture | TriggerVolume | Other
};

class PhysicsColliderComponent : public Component, public btMotionState
{
	DEPENDS_ON(TransformComponent);

	Ref<btCollisionShape> m_CollisionShape;
	Ref<btRigidBody> m_Body;
	btScalar m_Mass;
	Vector3 m_Gravity;
	Vector3 m_AngularFactor;
	Vector3 m_Offset;
	float m_Volume;
	bool m_IsMoveable;
	bool m_IsGeneratesHitEvents;
	bool m_IsKinematic;
	bool m_IsSleepable;
	bool m_IsCCD;
	unsigned int m_CollisionGroup;
	unsigned int m_CollisionMask;
	PhysicsMaterial m_Material;

	btVector3 m_LocalInertia;

	void getWorldTransform(btTransform& worldTrans) const override;
	void setWorldTransform(const btTransform& worldTrans) override;

	PhysicsColliderComponent(
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
	    bool canSleep,
	    bool isCCD,
	    const Ref<btCollisionShape>& collisionShape);

	friend class ECSFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::PhysicsColliderComponent;

	~PhysicsColliderComponent() = default;

	ComponentID getComponentID() const { return s_ID; }

	bool setupData() override;
	void onRemove() override;

	void applyForce(const Vector3& force);
	void applyTorque(const Vector3& torque);

	Vector3 getAngularFactor() const { return m_AngularFactor; }
	void setAngularFactor(const Vector3& factors);
	void setAxisLock(bool enabled);

	Vector3 getOffset() const { return m_Offset; };
	void setOffset(const Vector3& offset);

	Vector3 getGravity() const { return m_Gravity; };
	void setGravity(const Vector3& gravity);

	PhysicsMaterial getMaterial() const;

	Vector3 getVelocity();
	void setVelocity(const Vector3& velocity);

	Vector3 getAngularVelocity();
	void setAngularVelocity(const Vector3& angularVel);

	void translate(const Vector3& vec);

	void setTransform(const Matrix& mat);
	Matrix getTransform();

	bool isMoveable() { return m_IsMoveable; }
	void setMoveable(bool enabled);

	bool canSleep() { return m_IsSleepable; }
	void setSleepable(bool enabled);

	bool isCCD() { return m_IsCCD; }
	void setCCD(bool enabled);

	bool isGeneratesHitEvents() { return m_IsGeneratesHitEvents; }
	void setGeneratedHitEvents(bool enabled) { m_IsGeneratesHitEvents = enabled; }

	bool isKinematic() { return m_IsKinematic; }
	void setKinematic(bool enabled);

	virtual void highlight();

	virtual const char* getName() const override { return "PhysicsColliderComponent"; };
	virtual JSON::json getJSON() const override;

	virtual void draw() override;
	void displayCollisionLayers(unsigned int& collision);
};
