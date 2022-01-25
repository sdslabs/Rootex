#pragma once

#include "scene.h"
#include "resource_loader.h"
#include <os.h>

class ContentBrowser
{
	EventBinder<ContentBrowser> m_Binder;

public:
	struct ContentBrowserSettings
	{
		bool m_IsActive = true;
	};

private:
	ContentBrowserSettings m_ContentBrowserSettings;
	String m_AssetsDirectory = "game\\assets\\";
	String m_CurrentDirectory = m_AssetsDirectory;

	bool m_ReloadPending;

	Ref<ImageResourceFile> m_DirectoryImage;
	Ref<ImageResourceFile> m_ScriptImage;
	Ref<ImageResourceFile> m_MusicImage;

	Vector<FilePath> m_filepaths_cache;
	Vector<FilePath> m_dir_cache;
	HashMap<String, Ref<ImageResourceFile>> m_thumbnail_cache;

	static const inline HashMap<String, Vector<const char*>> m_PayloadTypes = {
		{ ".png", { "IMAGE_PAYLOAD" } },
		{ ".jpg", { "IMAGE_PAYLOAD" } },
		{ ".jpeg", { "IMAGE_PAYLOAD" } },
		{ ".dds", { "IMAGE_PAYLOAD", "IMAGE_CUBE_PAYLOAD" } },
		{ ".wav", { "AUDIO_PAYLOAD" } },
		{ ".3ds", { "MODEL_PAYLOAD" } },
		{ ".fbx", { "MODEL_PAYLOAD", "ANIMATED_MODEL_PAYLOAD" } },
		{ ".obj", { "MODEL_PAYLOAD", "COLLISION_MODEL_PAYLOAD" } },
		{ ".blend", { "MODEL_PAYLOAD" } },
		{ ".dae", { "MODEL_PAYLOAD", "ANIMATED_MODEL_PAYLOAD" } },
		{ ".gltf", { "ANIMATED_MODEL_PAYLOAD" } },
		{ ".lua", { "SCRIPT_PAYLOAD" } },
		{ ".rml", { "RML_PAYLOAD" } },
		{ ".rmat", { "MATERIAL_PAYLOAD" } },
		{ ".json", { "SCENE_PAYLOAD" } },
	};

	float m_IconWidth = 128;

public:
	ContentBrowser();
	ContentBrowser(const ContentBrowser&) = delete;
	~ContentBrowser() = default;

	void draw(float deltaMilliseconds);

	static ContentBrowser* GetSingleton();

	ContentBrowserSettings& getSettings() { return m_ContentBrowserSettings; }
	void setActive(bool enabled) { m_ContentBrowserSettings.m_IsActive = enabled; }
	//void CALLBACK notifyFileSystemChanges(PVOID lpParam, BOOLEAN TimerOrWaitFired);
};
