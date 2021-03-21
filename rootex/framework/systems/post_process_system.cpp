#include "post_process_system.h"

#include "systems/render_system.h"

PostProcessSystem::PostProcessSystem()
    : System("PostProcessSystem", UpdateOrder::PostRender, true)
{
}

PostProcessSystem* PostProcessSystem::GetSingleton()
{
	static PostProcessSystem singleton;
	return &singleton;
}

void PostProcessSystem::update(float deltaMilliseconds)
{
	m_Processor.draw(RenderSystem::GetSingleton()->getCamera());

	RenderingDevice::GetSingleton()->unbindSRVs();
	RenderingDevice::GetSingleton()->setOffScreenRTVDSV();
}
