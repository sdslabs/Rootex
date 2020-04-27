#pragma once

#include "component.h"
#include "components/transform_component.h"

#include "btBulletDynamicsCommon.h"

class PhysicsColliderComponent : public Component
{
	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::PhysicsColliderComponent;
	
	static btTransform matTobtTransform(Matrix const& mat);
	static Matrix btTransformToMat(btTransform const& trans);
	static btVector3 vecTobtVector3(Vector3 const& vec3);
	static Vector3 btVector3ToVec(btVector3 const& btvec);

	Ref<TransformComponent> m_TransformComponent;
	
	Ref<btCollisionShape> m_CollisionShape;
	btRigidBody* m_Body;
	Matrix m_Transform;
	btScalar m_Mass;
	btVector3 m_LocalInertia;
	float m_SpecificGravity;
	float m_Volume;
	bool m_IsMoveable;
	
#ifdef ROOTEX_EDITOR
	std::string m_MaterialName;
	Vector3 m_Gravity;
#endif // ROOTEX_EDITOR

	Color m_RenderColor;
	
	/// Interface that Bullet uses to communicate position and orientation changes.
	struct MotionState : public btMotionState
	{
		Matrix m_WorldToPositionTransform;

		MotionState(Matrix const& startingTransform);

		/// btMotionState interface:  Bullet calls these.
		virtual void getWorldTransform(btTransform& worldTrans) const;

		virtual void setWorldTransform(const btTransform& worldTrans);
	} m_MotionState;

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

	/// Helpers for conversion to and from Bullet's data types.
	PhysicsColliderComponent(const String& matName, float volume, const Vector3& gravity, bool isMoveable, const Ref<btCollisionShape>& collisionShape);
	~PhysicsColliderComponent() = default;

	ComponentID getComponentID() const { return s_ID; }

	bool setup() override;

	void applyForce(const Vector3& force);
	void applyTorque(const Vector3& torque);
	void setGravity(const Vector3& gravity);
	/// Forces a physics object to a new location/ orientation.
	void kinematicMove(const Matrix& matrix);
	void setVelocity(const Vector3& velocity);
	Vector3 getVelocity();
	void setAngularVelocity(const Vector3& angularVel);
	Vector3 getAngularVelocity();
	void translate(const Vector3& vec);
	/// Sets the current transform of the physics object.
	void setTransform(const Matrix& mat);
	/// Returns the current transform of the phyics object.
	Matrix getTransform();
	void setMoveable(bool enabled);

	virtual void render();

	virtual String getName() const override { return "PhysicsColliderComponent"; };
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif // ROOTEX_EDITOR
};
