#pragma once

#include "component.h"
#include "components/transform_component.h"
#include "components/script_component.h"

#include "btBulletDynamicsCommon.h"

/// Helpers for conversion to and from Bullet's data types.
btTransform MatTobtTransform(Matrix const& mat);
Matrix BtTransformToMat(btTransform const& trans);
btVector3 VecTobtVector3(Vector3 const& vec3);
Vector3 BtVector3ToVec(btVector3 const& btvec);

enum PhysicsMaterial;

class PhysicsColliderComponent : public Component, public btMotionState
{
	DEPENDS_ON(TransformComponent);
	SOFT_DEPENDS_ON(ScriptComponent);

	Ref<btCollisionShape> m_CollisionShape;
	Ref<btRigidBody> m_Body;
	btScalar m_Mass;
	Vector3 m_Gravity;
	float m_Volume;
	bool m_IsMoveable;
	bool m_IsGeneratesHitEvents;
	bool m_IsKinematic; 
	PhysicsMaterial m_Material;
	
	btVector3 m_LocalInertia;

	PhysicsColliderComponent(const PhysicsMaterial& material, float volume, const Vector3& gravity, bool isMoveable, bool isKinematic, bool generatesHitEvents, const Ref<btCollisionShape>& collisionShape);
	
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
	
	void onHit(btPersistentManifold* manifold, PhysicsColliderComponent* other);
	void applyForce(const Vector3& force);
	void applyTorque(const Vector3& torque);
	
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
#endif // ROOTEX_EDITOR
};
