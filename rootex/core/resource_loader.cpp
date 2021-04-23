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
	case ResourceFile::Type::BasicMaterial:
		return CreateBasicMaterialResourceFile(path);
	case ResourceFile::Type::InstancingBasicMaterial:
		return CreateInstancingBasicMaterialResourceFile(path);
	case ResourceFile::Type::AnimatedBasicMaterial:
		return CreateAnimatedBasicMaterialResourceFile(path);
	case ResourceFile::Type::SkyMaterial:
		return CreateSkyMaterialResourceFile(path);
	case ResourceFile::Type::CustomMaterial:
		return CreateCustomMaterialResourceFile(path);
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
	progress++;

	Application::GetSingleton()->getThreadPool().submit(tasks);

	return tasks.size(); // One less for the dummy task
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
		{ "rootex.root", "rootex.root" },
		{ "rootex/vendor/Debugger/Debugger.lua", "rootex/vendor/Debugger/Debugger.lua" },
		{ "game/startup.lua", "game/startup.lua" },
		{ "rootex/vendor/ASSAO/ASSAO.hlsl", "rootex/vendor/ASSAO/ASSAO.hlsl" },
		{ "rootex/vendor/Middleclass/Middleclass.lua", "rootex/vendor/Middleclass/Middleclass.lua" },
		{ "THIRDPARTY.md", "THIRDPARTY.md" },
		{ "rootex/vendor/FXAA/FXAA3_11.h", "rootex/vendor/FXAA/FXAA3_11.h" },
		{ "rootex/vendor/Flux/flux.lua", "rootex/vendor/Flux/flux.lua" }
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
	gameConfig["startScene"] = sceneFilePath;
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
	Vector<FilePath> shaderFiles = OS::GetAllFilesInDirectory("rootex/core/renderer/shaders");
	assetFiles.insert(assetFiles.end(), rootexAssetFiles.begin(), rootexAssetFiles.end());
	assetFiles.insert(assetFiles.end(), shaderFiles.begin(), shaderFiles.end());

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
	progress++;

	ThreadPool& threadPool = Application::GetSingleton()->getThreadPool();
	threadPool.submit(tasks);

	PRINT("Exporting to " + m_CurrExportDir)

	return tasks.size();
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

void ResourceLoader::Initialize()
{
	BasicMaterialResourceFile::Load();
	AnimatedBasicMaterialResourceFile::Load();
	InstancingBasicMaterialResourceFile::Load();
	SkyMaterialResourceFile::Load();
}

void ResourceLoader::Destroy()
{
	BasicMaterialResourceFile::Destroy();
	AnimatedBasicMaterialResourceFile::Destroy();
	InstancingBasicMaterialResourceFile::Destroy();
	SkyMaterialResourceFile::Destroy();
}

const HashMap<ResourceFile::Type, Vector<Weak<ResourceFile>>>& ResourceLoader::GetResources()
{
	return s_ResourcesDataFiles;
}

const char* ResourceLoader::GetCreatableExtension(ResourceFile::Type type)
{
	if (CreatableFiles.find(type) != CreatableFiles.end())
	{
		return CreatableFiles.at(type);
	}
	return nullptr;
}

void ResourceLoader::SaveResources(ResourceFile::Type type)
{
	for (auto& file : s_ResourcesDataFiles[type])
	{
		if (Ref<ResourceFile> resFile = file.lock())
		{
			if (resFile->getPath().string().substr(0, 6) != "rootex")
			{
				resFile->save();
			}
		}
	}
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

Ref<BasicMaterialResourceFile> ResourceLoader::CreateNewBasicMaterialResourceFile(const String& path)
{
	if (!OS::IsExists(path))
	{
		OS::CreateFileName(path);
	}
	return CreateBasicMaterialResourceFile(path);
}

Ref<AnimatedBasicMaterialResourceFile> ResourceLoader::CreateNewAnimatedBasicMaterialResourceFile(const String& path)
{
	if (!OS::IsExists(path))
	{
		OS::CreateFileName(path);
	}
	return CreateAnimatedBasicMaterialResourceFile(path);
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

Ref<MaterialResourceFile> ResourceLoader::CreateMaterialResourceFile(const String& path)
{
	String extension;
	int dotCount = 0;
	for (int ch = path.size() - 1; ch >= 0; ch--)
	{
		if (path[ch] == '.')
		{
			dotCount++;
			// When e.g. ".basic.rmat" has been found
			if (dotCount == 2)
			{
				extension = path.substr(ch, path.size());
				break;
			}
		}
	}

	if (extension.empty())
	{
		WARN("Couldn't deduce material type from file extension");
		return nullptr;
	}

	if (IsFileSupported(extension, ResourceFile::Type::BasicMaterial))
	{
		return CreateBasicMaterialResourceFile(path);
	}
	if (IsFileSupported(extension, ResourceFile::Type::AnimatedBasicMaterial))
	{
		return CreateAnimatedBasicMaterialResourceFile(path);
	}
	if (IsFileSupported(extension, ResourceFile::Type::InstancingBasicMaterial))
	{
		return CreateInstancingBasicMaterialResourceFile(path);
	}
	if (IsFileSupported(extension, ResourceFile::Type::SkyMaterial))
	{
		return CreateSkyMaterialResourceFile(path);
	}
	if (IsFileSupported(extension, ResourceFile::Type::CustomMaterial))
	{
		return CreateCustomMaterialResourceFile(path);
	}

	return nullptr;
}

Ref<BasicMaterialResourceFile> ResourceLoader::CreateBasicMaterialResourceFile(const String& path)
{
	return GetCachedResource<BasicMaterialResourceFile>(ResourceFile::Type::BasicMaterial, FilePath(path));
}

Ref<InstancingBasicMaterialResourceFile> ResourceLoader::CreateInstancingBasicMaterialResourceFile(const String& path)
{
	return GetCachedResource<InstancingBasicMaterialResourceFile>(ResourceFile::Type::InstancingBasicMaterial, FilePath(path));
}

Ref<AnimatedBasicMaterialResourceFile> ResourceLoader::CreateAnimatedBasicMaterialResourceFile(const String& path)
{
	return GetCachedResource<AnimatedBasicMaterialResourceFile>(ResourceFile::Type::AnimatedBasicMaterial, FilePath(path));
}

Ref<SkyMaterialResourceFile> ResourceLoader::CreateSkyMaterialResourceFile(const String& path)
{
	return GetCachedResource<SkyMaterialResourceFile>(ResourceFile::Type::SkyMaterial, FilePath(path));
}

Ref<CustomMaterialResourceFile> ResourceLoader::CreateCustomMaterialResourceFile(const String& path)
{
	return GetCachedResource<CustomMaterialResourceFile>(ResourceFile::Type::CustomMaterial, FilePath(path));
}
