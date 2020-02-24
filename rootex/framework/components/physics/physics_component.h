#pragma once

#include "component.h"
#include "components/transform_component.h"

#include "btBulletDynamicsCommon.h"

class PhysicsComponent : public Component
{
	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::PhysicsComponent;

	ComponentID getComponentID() const { return s_ID; }

	static btTransform matTobtTransform(Matrix const& mat);
	static Matrix btTransformToMat(btTransform const& trans);
	static btVector3 vecTobtVector3(Vector3 const& vec3);
	static Vector3 btVector3ToVec(btVector3 const& btvec);
	PhysicsComponent(const String& matName, float volume, const Ref<btCollisionShape>& collisionShape);
	~PhysicsComponent() = default;

	struct MotionState : public btMotionState
	{
		Matrix m_WorldToPositionTransform;

		MotionState(Matrix const& startingTransform);

		// btMotionState interface:  Bullet calls these
		virtual void getWorldTransform(btTransform& worldTrans) const;

		virtual void setWorldTransform(const btTransform& worldTrans);
	} m_MotionState;

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

	Ref<btCollisionShape> m_CollisionShape;
	btRigidBody* m_Body;

	TransformComponent* m_TransformComponent;
	bool setup() override;

	Matrix transform;
	btScalar m_Mass;
	btVector3 localInertia;
	float m_SpecificGravity;
	float m_Volume;
	LuaInterpreter physicsLua;
	LuaTextResourceFile* physicsMaterial;
	std::string m_MaterialName;

	void applyForce(const Vector3 force);
	void applyTorque(const Vector3 torque);
	// Forces a physics object to a new location/orientation
	void kinematicMove(const Matrix& matrix);
	void setVelocity(const Vector3& velocity);
	Vector3 getVelocity();
	void setAngularVelocity(const Vector3& angularVel);
	Vector3 getAngularVelocity();
	void translate(const Vector3& vec);
	//    Forces a phyics object to a new location/orientation
	void setTransform(const Matrix& mat);
	//   Returns the current transform of the phyics object
	Matrix getTransform();
};
