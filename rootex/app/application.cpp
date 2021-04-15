#include "application.h"

#include "framework/scene_loader.h"
#include "framework/ecs_factory.h"
#include "core/resource_loader.h"
#include "core/resource_files/lua_text_resource_file.h"
#include "core/input/input_manager.h"
#include "script/interpreter.h"

#include "systems/audio_system.h"
#include "systems/physics_system.h"
#include "systems/input_system.h"
#include "systems/ui_system.h"
#include "systems/render_ui_system.h"
#include "systems/render_system.h"
#include "systems/particle_system.h"
#include "systems/post_process_system.h"
#include "systems/script_system.h"
#include "systems/transform_animation_system.h"
#include "systems/trigger_system.h"
#include "systems/player_system.h"

#include "Tracy/Tracy.hpp"

Application* Application::s_Singleton = nullptr;

String Application::getSaveSlotPath(int slot)
{
	return OS::GetAbsoluteSaveGameFolder(getAppTitle()) + "/save_" + std::to_string(slot) + ".json";
}

Application* Application::GetSingleton()
{
	return s_Singleton;
}

Application::Application(const String& appTitle, const String& settingsFile)
    : m_ApplicationTitle(appTitle)
{
	if (!s_Singleton)
	{
		s_Singleton = this;
	}
	else
	{
		ERR("More than one Rootex applications are active");
	}

	if (!OS::Initialize())
	{
		ERR("Application OS was not initialized");
	}

	if (!OS::CreateDirectoryAbsoluteName(OS::GetAbsoluteSaveGameFolder(getAppTitle())))
	{
		WARN("Could not create save slots folder");
	}

	m_ApplicationSettings.reset(new ApplicationSettings(ResourceLoader::CreateTextResourceFile(settingsFile)));

	const JSON::json& splashSettings = m_ApplicationSettings->getJSON()["splash"];
	m_SplashWindow.reset(new SplashWindow(
	    splashSettings["title"],
	    splashSettings["icon"],
	    splashSettings["image"],
	    splashSettings["width"],
	    splashSettings["height"]));

	PANIC(!OS::ElevateThreadPriority(), "Could not elevate main thread priority");
	PRINT("Current main thread priority: " + std::to_string(OS::GetCurrentThreadPriority()));

	ECSFactory::Initialize();

	JSON::json& systemsSettings = m_ApplicationSettings->getJSON()["systems"];
	LuaInterpreter::GetSingleton();

	JSON::json windowJSON = m_ApplicationSettings->getJSON()["window"];
	m_Window.reset(new Window(
	    windowJSON["x"],
	    windowJSON["y"],
	    OS::GetDisplayWidth(),
	    OS::GetDisplayHeight(),
	    windowJSON["title"],
	    windowJSON["isEditor"],
	    windowJSON["fullScreen"],
	    windowJSON["icon"]));
	JSON::json& inputSystemSettings = systemsSettings["InputSystem"];
	inputSystemSettings["width"] = m_Window->getWidth();
	inputSystemSettings["height"] = m_Window->getHeight();
	InputSystem::GetSingleton()->initialize(inputSystemSettings);

	ResourceLoader::Initialize();
	PhysicsSystem::GetSingleton()->initialize(systemsSettings["PhysicsSystem"]);
	TriggerSystem::GetSingleton();

	JSON::json& uiSystemSettings = systemsSettings["UISystem"];
	uiSystemSettings["width"] = m_Window->getWidth();
	uiSystemSettings["height"] = m_Window->getHeight();
	UISystem::GetSingleton()->initialize(uiSystemSettings);

	RenderUISystem::GetSingleton();
	RenderSystem::GetSingleton();
	ParticleSystem::GetSingleton()->initialize(systemsSettings["ParticleSystem"]);
	PostProcessSystem::GetSingleton();

	TransformAnimationSystem::GetSingleton();
	AnimationSystem::GetSingleton();

	ScriptSystem::GetSingleton();

	if (!AudioSystem::GetSingleton()->initialize(systemsSettings["AudioSystem"]))
	{
		ERR("Audio System was not initialized");
	}

	PlayerSystem::GetSingleton();

	auto&& postInitialize = m_ApplicationSettings->find("postInitialize");
	if (postInitialize != m_ApplicationSettings->end())
	{
		try
		{
			LuaInterpreter::GetSingleton()->getLuaState().script_file(*postInitialize);
		}
		catch (std::exception e)
		{
			ERR("Error during post initialization: " + e.what());
		}
	}

	m_Window->show();
}

Application::~Application()
{
	SceneLoader::GetSingleton()->destroyAllScenes();
	AudioSystem::GetSingleton()->shutDown();
	UISystem::GetSingleton()->shutDown();
	ResourceLoader::Destroy();
	EventManager::GetSingleton()->releaseAllEventListeners();
}

void Application::run()
{
	while (!m_Window->processMessages())
	{
		m_FrameTimer.reset();

		for (auto& systems : System::GetSystems())
		{
			for (auto& system : systems)
			{
				if (system->isActive())
				{
					system->update(m_DeltaMultiplier * m_FrameTimer.getLastFrameTime());
				}
			}
		}

		process(m_FrameTimer.getLastFrameTime());

		EventManager::GetSingleton()->dispatchDeferred();

		m_Window->swapBuffers();
		FrameMark;
	}

	EventManager::GetSingleton()->call(RootexEvents::ApplicationExit);
}

void Application::process(float deltaMilliseconds)
{
	// Unused process function, meaning app doesn't need
	// extra logic running every frame apart from the systems running
}

void Application::end()
{
}

void Application::createSaveSlot(int slot)
{
	String slotPath = getSaveSlotPath(slot);
	if (OS::CreateFileNameAbsolute(slotPath))
	{
		PRINT("Created save:" + slotPath);
	}
	else
	{
		WARN("Could not create save: " + slotPath);
	}

	OS::SaveFileAbsolute(slotPath, "{}", 3);
}

bool Application::loadSave(int slot)
{
	String savePath = getSaveSlotPath(slot);
	if (!OS::IsExistsAbsolute(savePath))
	{
		PRINT("Save slot doesn't exist. Creating new one at slot " + std::to_string(slot) + ": " + savePath);
		createSaveSlot(slot);
	}

	try
	{
		m_CurrentSaveData = JSON::json::parse(OS::LoadFileContents(savePath));
		m_CurrentSaveSlot = slot;
	}
	catch (std::exception e)
	{
		ERR("Could not load save slot" + std::to_string(slot) + " " + e.what());
		return false;
	}
	return true;
}

JSON::json& Application::getSaveData()
{
	return m_CurrentSaveData;
}

bool Application::saveSlot()
{
	String saveSlotContents = m_CurrentSaveData.dump(4);
	return OS::SaveFileAbsolute(getSaveSlotPath(m_CurrentSaveSlot), saveSlotContents.c_str(), saveSlotContents.size());
}

Vector<FilePath> Application::getLibrariesPaths()
{
	return OS::GetDirectoriesInDirectory("rootex/vendor/");
}

void Application::destroySplashWindow()
{
	m_SplashWindow.reset();
}
