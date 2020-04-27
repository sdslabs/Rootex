#pragma once

#include "common/common.h"
#include "core/renderer/materials/color_material.h"
#include "Bullet3D/src/LinearMath/btIDebugDraw.h"

class DebugDrawer : public btIDebugDraw
{
	ColorMaterial m_ColorMaterial;

public:
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
	virtual void reportErrorWarning(const char* warningString) override;
	virtual void draw3dText(const btVector3& location, const char* textString) override;
	virtual void setDebugMode(int debugMode) override;
	virtual int getDebugMode() const override;
};
