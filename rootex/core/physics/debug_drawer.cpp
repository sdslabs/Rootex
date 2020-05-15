#include "debug_drawer.h"

#include "framework/systems/render_system.h"

DebugDrawer::DebugDrawer()
{
	m_BasicDefaultMaterial = std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetDefaultMaterial());
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	VertexBuffer vb({ 
		from.x(), from.y(), from.z(),
	    to.x(), to.y(), to.z() });
	IndexBuffer ib({ 0, 1 });
	m_BasicDefaultMaterial->setColor({ color.x(), color.y(), color.z(), 1.0f });

	RenderSystem::GetSingleton()->enableLineRenderMode();
	RenderSystem::GetSingleton()->getRenderer()->draw(&vb, &ib, m_BasicDefaultMaterial.get());
	RenderSystem::GetSingleton()->resetRenderMode();
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
