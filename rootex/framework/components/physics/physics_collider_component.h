#pragma once

#include "component.h"
#include "components/transform_component.h"
#include "components/script_component.h"

#include "btBulletDynamicsCommon.h"

class PhysicsColliderComponent : public Component, public btMotionState
{
	DEPENDS_ON(TransformComponent);
	SOFT_DEPENDS_ON(ScriptComponent);

	Ref<btCollisionShape> m_CollisionShape;
	Ref<btRigidBody> m_Body;
	btScalar m_Mass;
	btVector3 m_LocalInertia;
	float m_SpecificGravity;
	float m_Volume;
	bool m_IsMoveable;
	bool m_IsGeneratesHitEvents;
	
#ifdef ROOTEX_EDITOR
	std::string m_MaterialName;
	Vector3 m_Gravity;
#endif // ROOTEX_EDITOR
	
	/// Stores material specific details.
	struct MaterialData
	{
		float m_Restitution;
		float m_Friction;

		MaterialData(float restitution, float friction)
		{
			m_Restitution = restitution;
			m_Friction = friction;
		}
	} m_Material;

	Color m_RenderColor;
	
	friend class ECSFactory;

public:
	static void RegisterAPI(sol::table& rootex);
	static const ComponentID s_ID = (ComponentID)ComponentIDs::PhysicsColliderComponent;
	
	/// Helpers for conversion to and from Bullet's data types.
	static btTransform matTobtTransform(Matrix const& mat);
	static Matrix btTransformToMat(btTransform const& trans);
	static btVector3 vecTobtVector3(Vector3 const& vec3);
	static Vector3 btVector3ToVec(btVector3 const& btvec);
	
	virtual void getWorldTransform(btTransform& worldTrans) const;
	virtual void setWorldTransform(const btTransform& worldTrans);
	
	PhysicsColliderComponent(const String& matName, float volume, const Vector3& gravity, bool isMoveable, const Ref<btCollisionShape>& collisionShape, bool generatesHitEvents);
	~PhysicsColliderComponent() = default;

	ComponentID getComponentID() const { return s_ID; }

	bool setupData() override;
	void onRemove() override;
	
	void onHit(btPersistentManifold* manifold, PhysicsColliderComponent* other);

	void applyForce(const Vector3& force);
	void applyTorque(const Vector3& torque);
	
	void setGravity(const Vector3& gravity);
	
	/// Forces a physics object to a new location/ orientation.
	void kinematicMove(const Matrix& matrix);
	
	Vector3 getVelocity();
	void setVelocity(const Vector3& velocity);
	
	Vector3 getAngularVelocity();
	void setAngularVelocity(const Vector3& angularVel);
	
	void translate(const Vector3& vec);
	
	/// Sets the current transform of the physics object.
	void setTransform(const Matrix& mat);
	/// Returns the current transform of the phyics object.
	Matrix getTransform();
	
	bool getIsMoveable() { return m_IsMoveable; }
	void setMoveable(bool enabled);

	bool isGeneratesHitEvents() { return m_IsGeneratesHitEvents; }
	void setGeneratedHitEvents(bool enabled) { m_IsGeneratesHitEvents = enabled; }

	virtual void render();

	virtual const char* getName() const override { return "PhysicsColliderComponent"; };
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif // ROOTEX_EDITOR
};
