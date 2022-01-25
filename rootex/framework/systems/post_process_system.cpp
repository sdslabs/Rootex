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
	CameraComponent* camera = RenderSystem::GetSingleton()->getCamera();
	m_PostProcessingDetails = camera->getPostProcessingDetails();

	if (m_PostProcessingDetails.customPostProcessing.find(path) == m_PostProcessingDetails.customPostProcessing.end())
	{
		camera->addCustomPostProcessingDetails(path);
		m_Processor.m_PostProcesses.emplace_back(new CustomPostProcess(path));
	}
}
