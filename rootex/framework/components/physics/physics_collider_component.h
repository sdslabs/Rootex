#pragma once

#include "component.h"
#include "components/transform_component.h"

#include "btBulletDynamicsCommon.h"

/// Helpers for conversion to and from Bullet's data types.
btTransform MatTobtTransform(Matrix const& mat);
Matrix BtTransformToMat(btTransform const& trans);
btVector3 VecTobtVector3(Vector3 const& vec3);
Vector3 BtVector3ToVec(btVector3 const& btvec);

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
	float m_Volume;
	bool m_IsMoveable;
	bool m_IsGeneratesHitEvents;
	bool m_IsKinematic;
	unsigned int m_CollisionGroup;
	unsigned int m_CollisionMask;
	PhysicsMaterial m_Material;
	
	btVector3 m_LocalInertia;

	PhysicsColliderComponent(const PhysicsMaterial& material, float volume, const Vector3& gravity, const Vector3& angularFactor, int collisionGroup, int collisionMask, bool isMoveable, bool isKinematic, bool generatesHitEvents, const Ref<btCollisionShape>& collisionShape);
	
	friend class ECSFactory;

public:
	static void RegisterAPI(sol::table& rootex);
	static const ComponentID s_ID = (ComponentID)ComponentIDs::PhysicsColliderComponent;
	
	~PhysicsColliderComponent() = default;

	ComponentID getComponentID() const { return s_ID; }

	bool setupData() override;
	void onRemove() override;

	virtual void getWorldTransform(btTransform& worldTrans) const;
	virtual void setWorldTransform(const btTransform& worldTrans);
	
	void applyForce(const Vector3& force);
	void applyTorque(const Vector3& torque);
	
	Vector3 getAngularFactor() const { return m_AngularFactor; }
	void setAngularFactor(const Vector3& factors);
	void setAxisLock(bool enabled);

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
	
	bool isGeneratesHitEvents() { return m_IsGeneratesHitEvents; }
	void setGeneratedHitEvents(bool enabled) { m_IsGeneratesHitEvents = enabled; }
	
	bool isKinematic() { return m_IsKinematic; }
	void setKinematic(bool enabled);

	virtual void render();

	virtual const char* getName() const override { return "PhysicsColliderComponent"; };
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
	void displayCollisionLayers(unsigned int& collision);
#endif // ROOTEX_EDITOR
};
