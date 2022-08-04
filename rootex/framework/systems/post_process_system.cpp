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

void PostProcessSystem::addCustomPostProcessing(const String& path)
{
	for (auto&& postProcess : m_Processor.m_PostProcesses)
	{
		CustomPostProcess* customPostProcess = dynamic_cast<CustomPostProcess*>(postProcess.get());
		if (customPostProcess && customPostProcess->m_PostProcessPath == path)
		{
 			return;
		}
	}

	m_Processor.m_PostProcesses.emplace_back(new CustomPostProcess(path));
}
