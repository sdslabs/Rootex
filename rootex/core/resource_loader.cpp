#include "resource_loader.h"

#include "app/application.h"
#include "os/thread.h"

bool IsFileSupported(const String& extension, ResourceFile::Type supportedFileType)
{
	if (extension.empty())
	{
		return false;
	}
	String extensions(SupportedFiles.at(supportedFileType));
	return extensions.find(extension) != String::npos;
}

Ref<ResourceFile> ResourceLoader::CreateResourceFile(const ResourceFile::Type& type, const String& path)
{
	if (SupportedFiles.find(type) == SupportedFiles.end())
	{
		WARN("Tried to load an unsupported file type: " + path + " of type " + ResourceFile::s_TypeNames.at(type));
		return nullptr;
	}

	switch (type)
	{
	case ResourceFile::Type::Text:
		return CreateTextResourceFile(path);
	case ResourceFile::Type::Audio:
		return CreateAudioResourceFile(path);
	case ResourceFile::Type::Font:
		return CreateFontResourceFile(path);
	case ResourceFile::Type::Image:
		return CreateImageResourceFile(path);
	case ResourceFile::Type::Lua:
		return CreateLuaTextResourceFile(path);
	case ResourceFile::Type::Model:
		return CreateModelResourceFile(path);
	case ResourceFile::Type::AnimatedModel:
		return CreateAnimatedModelResourceFile(path);
	case ResourceFile::Type::CollisionModel:
		return CreateCollisionModelResourceFile(path);
	// Not in use due to threading issues
	case ResourceFile::Type::ParticleEffect:
	default:
		break;
	}

	return nullptr;
}

int ResourceLoader::Preload(ResourceCollection paths, Atomic<int>& progress)
{
	progress = 0;

	if (paths.empty())
	{
		PRINT("Asked to reload an empty list of files. Did nothing.");
		return 0;
	}

	std::sort(paths.begin(), paths.end(), [](const Pair<ResourceFile::Type, String>& a, const Pair<ResourceFile::Type, String>& b) {
		return (int)a.first < (int)b.first && a.second < b.second;
	});

	ResourceCollection empericalPaths = { paths.front() };
	for (auto& incomingPath : paths)
	{
		if (empericalPaths.back() != incomingPath)
		{
			empericalPaths.emplace_back(incomingPath);
		}
	}

	Vector<Ref<Task>> tasks;

	for (auto& path : empericalPaths)
	{
		ResourceFile::Type type = path.first;
		String pathString = path.second;

		tasks.push_back(std::make_shared<Task>([=, &progress]() {
			Ref<ResourceFile> res = CreateResourceFile(type, pathString);
			Persist(res);
			progress++;
		}));
	}

	/// TODO: Fix the need for this dummy task (blocks the main thread if no resources are preloaded)
	tasks.push_back(std::make_shared<Task>([]() {}));

	Application::GetSingleton()->getThreadPool().submit(tasks);

	return tasks.size() - 1; // One less for the dummy task
}

#ifdef RELEASE_BUILD
int ResourceLoader::Export(const String& sceneName, const String& sceneFilePath, Atomic<int>& progress)
{
	Vector<Pair<String, String>> toCopy = {
		{ "build/game/Release/Game.exe",
		    "Game.exe" },
		{ "build/game/Release/alut.dll",
		    "alut.dll" },
		{ "build/game/Release/OpenAL32.dll",
		    "OpenAL32.dll" },
		{ "build/game/Release/wrap_oal.dll",
		    "wrap_oal.dll" },
		{ "rootex.root", "rootex.root" },
		{ "rootex/vendor/Debugger/Debugger.lua", "rootex/vendor/Debugger/Debugger.lua" },
		{ "game/startup.lua", "game/startup.lua" },
		{ "rootex/vendor/ASSAO/ASSAO.hlsl", "rootex/vendor/ASSAO/ASSAO.hlsl" },
		{ "rootex/vendor/Middleclass/Middleclass.lua", "rootex/vendor/Middleclass/Middleclass.lua" },
		{ "THIRDPARTY.md", "THIRDPARTY.md" }
	};

	int i = 0;
	do
	{
		m_CurrExportDir = "exports/" + sceneName;
		if (i != 0)
		{
			m_CurrExportDir += std::to_string(i);
		}
		m_CurrExportDir += "/";
		i++;
	} while (OS::IsExists(m_CurrExportDir));

	OS::CreateDirectoryName(m_CurrExportDir);
	OS::CreateDirectoryName(m_CurrExportDir + "game/assets/");
	OS::CreateDirectoryName(m_CurrExportDir + "rootex/assets/");

	JSON::json gameConfig = JSON::json::parse(ResourceLoader::CreateTextResourceFile("game/game.app.json")->getString());
	gameConfig["startLevel"] = sceneFilePath;
	Ref<TextResourceFile> newGameConfig = ResourceLoader::CreateNewTextResourceFile(m_CurrExportDir + "game/game.app.json");
	newGameConfig->putString(gameConfig.dump(4));

	if (!newGameConfig->save())
	{
		WARN("Could not save application settings file");
		OS::DeleteDirectory(m_CurrExportDir);
		return 0;
	}

	Ref<TextResourceFile> readme = ResourceLoader::CreateNewTextResourceFile(m_CurrExportDir + "readme.txt");
	readme->putString("This Game was build using Rootex Game Engine. Find the source code here http://github.com/SDSLabs/Rootex.");
	if (!readme->save())
	{
		WARN("Could not save readme file");
		OS::DeleteDirectory(m_CurrExportDir);
		return 0;
	}

	Vector<FilePath> assetFiles = OS::GetAllFilesInDirectory("game/assets/");
	Vector<FilePath> rootexAssetFiles = OS::GetAllFilesInDirectory("rootex/assets/");
	assetFiles.insert(assetFiles.end(), rootexAssetFiles.begin(), rootexAssetFiles.end());

	for (auto& file : assetFiles)
	{
		toCopy.push_back({ file.generic_string(), file.generic_string() });
	}

	Vector<Ref<Task>> tasks;
	m_CopyFailed = false;

	for (auto& filePair : toCopy)
	{
		tasks.push_back(std::make_shared<Task>([=, &progress]() {
			if (m_CopyFailed)
			{
				return;
			}
			m_CopyFailed = !OS::RelativeCopyFile(filePair.first, m_CurrExportDir + filePair.second);
			if (m_CopyFailed)
			{
				ERR_SILENT("Could not copy asset files, investigate console logs ");
				OS::DeleteDirectory(m_CurrExportDir);
			}
			progress++;
		}));
	}

	/// TODO: Fix the need for this dummy task (blocks the main thread while tasks are running)
	tasks.push_back(std::make_shared<Task>([]() {}));

	ThreadPool& threadPool = Application::GetSingleton()->getThreadPool();
	threadPool.submit(tasks);

	PRINT("Exporting to " + m_CurrExportDir)

	return toCopy.size();
}

void ResourceLoader::PostExport()
{
	PRINT("Successfully exported to " + m_CurrExportDir);
	m_CurrExportDir = "";
}
#endif

void ResourceLoader::Persist(Ref<ResourceFile> res)
{
	s_PersistMutex.lock();
	s_PersistentResources.push_back(res);
	s_PersistMutex.unlock();
}

void ResourceLoader::ClearPersistentResources()
{
	s_PersistentResources.clear();
}

const HashMap<ResourceFile::Type, Vector<Weak<ResourceFile>>>& ResourceLoader::GetResources()
{
	return s_ResourcesDataFiles;
}

void ResourceLoader::ClearDeadResources()
{
	for (auto& [type, resources] : s_ResourcesDataFiles)
	{
		Vector<int> toRemove;
		for (int i = 0; i < resources.size(); i++)
		{
			if (!resources[i].lock())
			{
				resources.erase(resources.begin() + i);
				i--;
			}
		}
	}
}

Ref<TextResourceFile> ResourceLoader::CreateTextResourceFile(const String& path)
{
	return GetCachedResource<TextResourceFile>(ResourceFile::Type::Text, FilePath(path));
}

Ref<TextResourceFile> ResourceLoader::CreateNewTextResourceFile(const String& path)
{
	if (!OS::IsExists(path))
	{
		OS::CreateFileName(path);
	}
	return CreateTextResourceFile(path);
}

Ref<LuaTextResourceFile> ResourceLoader::CreateLuaTextResourceFile(const String& path)
{
	return GetCachedResource<LuaTextResourceFile>(ResourceFile::Type::Lua, FilePath(path));
}

Ref<AudioResourceFile> ResourceLoader::CreateAudioResourceFile(const String& path)
{
	return GetCachedResource<AudioResourceFile>(ResourceFile::Type::Audio, FilePath(path));
}

Ref<ModelResourceFile> ResourceLoader::CreateModelResourceFile(const String& path)
{
	return GetCachedResource<ModelResourceFile>(ResourceFile::Type::Model, FilePath(path));
}

Ref<CollisionModelResourceFile> ResourceLoader::CreateCollisionModelResourceFile(const String& path)
{
	return GetCachedResource<CollisionModelResourceFile>(ResourceFile::Type::CollisionModel, FilePath(path));
}

Ref<AnimatedModelResourceFile> ResourceLoader::CreateAnimatedModelResourceFile(const String& path)
{
	return GetCachedResource<AnimatedModelResourceFile>(ResourceFile::Type::AnimatedModel, FilePath(path));
}

Ref<ImageResourceFile> ResourceLoader::CreateImageResourceFile(const String& path)
{
	return GetCachedResource<ImageResourceFile>(ResourceFile::Type::Image, FilePath(path));
}

Ref<ImageCubeResourceFile> ResourceLoader::CreateImageCubeResourceFile(const String& path)
{
	return GetCachedResource<ImageCubeResourceFile>(ResourceFile::Type::ImageCube, FilePath(path));
}

Ref<FontResourceFile> ResourceLoader::CreateFontResourceFile(const String& path)
{
	return GetCachedResource<FontResourceFile>(ResourceFile::Type::Font, FilePath(path));
}

Ref<ParticleEffectResourceFile> ResourceLoader::CreateParticleEffectResourceFile(const String& path)
{
	return GetCachedResource<ParticleEffectResourceFile>(ResourceFile::Type::ParticleEffect, FilePath(path));
}
