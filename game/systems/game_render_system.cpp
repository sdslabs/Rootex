#include "game_render_system.h"

#include "renderer/rendering_device.h"

GameRenderSystem::GameRenderSystem()
    : System("GameRenderSystem", System::UpdateOrder::GameRender, true)
{
}

GameRenderSystem* GameRenderSystem::GetSingleton()
{
	static GameRenderSystem singleton;
	return &singleton;
}

bool GameRenderSystem::initialize(const JSON::json& systemData)
{
	m_BasicPostProcess.reset(new DirectX::BasicPostProcess(RenderingDevice::GetSingleton()->getDevice()));
	return true;
}

void GameRenderSystem::update(float deltaMilliseconds)
{
	RenderingDevice::GetSingleton()->unbindSRVs();
	RenderingDevice::GetSingleton()->setMainRT();

	m_BasicPostProcess->SetSourceTexture(RenderingDevice::GetSingleton()->getOffScreenSRV().Get());
	m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Copy);
	m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());
}
