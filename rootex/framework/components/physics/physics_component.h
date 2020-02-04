#pragma once

#include "component.h"
#include"components/transform_component.h"

#include"btBulletDynamicsCommon.h"

class PhysicsComponent : public Component
{
	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::PhysicsComponent;

	ComponentID getComponentID() const { return s_ID; }

	PhysicsComponent(btScalar mass, std::string& matName);
	~PhysicsComponent();

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

		MaterialData(float restitution, float friction);
	} m_Material;

	Ptr<btCollisionShape> collisionShape;
	btRigidBody* body;

	TransformComponent* m_TransformComponent;
	bool setup() override;
 
	Matrix transform;
	btScalar mass;
	btVector3 localInertia;	
	float specificGravity;
	float volume;
	LuaInterpreter physicsLua;
	LuaTextResourceFile* physicsMaterial;
	std::string m_MaterialName;

	static btTransform matTobtTransform(Matrix const& mat);
	static Matrix btTransformToMat(btTransform const& trans);
	static btVector3 vecTobtVector3(Vector3 const& vec3);
	static Vector3 btVector3ToVec(btVector3 const& btvec);
	float lookUpSpecificGravity(const std::string& matName);
	MaterialData lookUpMaterialData(const std::string& materialName); 
	void applyForce(const Vector3 force);
	void applyTorque(const Vector3 torque);
	void setVelocity(const Vector3& velocity);
	Vector3 getVelocity();
	void setAngularVelocity(const Vector3& angularVel);
	Vector3 getAngularVelocity();
	void translate(const Vector3& vec);
};
