#include "debug_drawer.h"

#include "framework/systems/render_system.h"
#include "renderer/materials/basic_material.h"

DebugDrawer::DebugDrawer()
{
	m_BasicMaterial = std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetMaterial("rootex/assets/materials/collision.rmat"));
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	RenderSystem::GetSingleton()->submitLine({ from.x(), from.y(), from.z() }, { to.x(), to.y(), to.z() });
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	WARN("Bullet3 Warning: " + String(warningString));
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
}

void DebugDrawer::setDebugMode(int debugMode)
{
}

int DebugDrawer::getDebugMode() const
{
	return btIDebugDraw::DBG_DrawWireframe;
}
