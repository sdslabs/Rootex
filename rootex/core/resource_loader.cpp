#include "resource_loader.h"

#include "app/application.h"
#include "os/thread.h"

HashMap<ResourceFile::Type, Vector<Ref<ResourceFile>>> ResourceLoader::s_ResourcesDataFiles;

bool IsFileSupported(const String& extension, ResourceFile::Type supportedFileType)
{
	if (extension.empty())
	{
		return false;
	}
	String extensions(SupportedFiles.at(supportedFileType));
	return extensions.find(extension) != String::npos;
}

ResourceFile* ResourceLoader::CreateSomeResourceFile(const String& path)
{
	ResourceFile* result = nullptr;
	for (auto& [resourceType, extensions] : SupportedFiles)
	{
		String extensionsString(extensions);
		if (extensionsString.find(FilePath(path).extension().generic_string()) != String::npos)
		{
			switch (resourceType)
			{
			case ResourceFile::Type::Text:
				result = CreateTextResourceFile(path);
				break;
			case ResourceFile::Type::Audio:
				result = CreateAudioResourceFile(path);
				break;
			case ResourceFile::Type::Font:
				result = CreateFontResourceFile(path);
				break;
			case ResourceFile::Type::Image:
				result = CreateImageResourceFile(path);
				break;
			case ResourceFile::Type::Lua:
				result = CreateLuaTextResourceFile(path);
				break;
			case ResourceFile::Type::Model:
				result = CreateModelResourceFile(path);
				break;
			default:
				break;
			}
		}
	}
	return result;
}

void ResourceLoader::RegisterResource(Ref<ResourceFile> file)
{
	s_ResourcesDataFiles[file->getType()].push_back(file);
}

void ResourceLoader::RegisterAPI(sol::table& rootex)
{
	sol::usertype<ResourceLoader> resourceLoader = rootex.new_usertype<ResourceLoader>("ResourceLoader");
	resourceLoader["CreateAudio"] = &ResourceLoader::CreateAudioResourceFile;
	resourceLoader["CreateFont"] = &ResourceLoader::CreateFontResourceFile;
	resourceLoader["CreateImage"] = &ResourceLoader::CreateImageResourceFile;
	resourceLoader["CreateLua"] = &ResourceLoader::CreateLuaTextResourceFile;
	resourceLoader["CreateText"] = &ResourceLoader::CreateTextResourceFile;
	resourceLoader["CreateNewText"] = &ResourceLoader::CreateNewTextResourceFile;
	resourceLoader["CreateVisualModel"] = &ResourceLoader::CreateModelResourceFile;
	resourceLoader["CreateAnimatedModel"] = &ResourceLoader::CreateAnimatedModelResourceFile;
}

TextResourceFile* ResourceLoader::CreateTextResourceFile(const String& path)
{
	return GetCachedResource<TextResourceFile>(ResourceFile::Type::Text, FilePath(path));
}

TextResourceFile* ResourceLoader::CreateNewTextResourceFile(const String& path)
{
	if (!OS::IsExists(path))
	{
		OS::CreateFileName(path);
	}
	return CreateTextResourceFile(path);
}

LuaTextResourceFile* ResourceLoader::CreateLuaTextResourceFile(const String& path)
{
	return GetCachedResource<LuaTextResourceFile>(ResourceFile::Type::Lua, FilePath(path));
}

AudioResourceFile* ResourceLoader::CreateAudioResourceFile(const String& path)
{
	return GetCachedResource<AudioResourceFile>(ResourceFile::Type::Audio, FilePath(path));
}

ModelResourceFile* ResourceLoader::CreateModelResourceFile(const String& path)
{
	return GetCachedResource<ModelResourceFile>(ResourceFile::Type::Model, FilePath(path));
}

AnimatedModelResourceFile* ResourceLoader::CreateAnimatedModelResourceFile(const String& path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::AnimatedModel)
		{
			return reinterpret_cast<AnimatedModelResourceFile*>(item.second.get());
		}
	}
	if (OS::IsExists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	// File not found in cache, load it only once
	FileBuffer& buffer = OS::LoadFileContents(path);
	ResourceData* resData = new ResourceData(path, buffer);
	AnimatedModelResourceFile* animationRes = new AnimatedModelResourceFile(resData);

	LoadAssimp(animationRes);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(animationRes);

	return animationRes;
}

ImageResourceFile* ResourceLoader::CreateImageResourceFile(const String& path)
{
	return GetCachedResource<ImageResourceFile>(ResourceFile::Type::Image, FilePath(path));
}

ImageCubeResourceFile* ResourceLoader::CreateImageCubeResourceFile(const String& path)
{
	return GetCachedResource<ImageCubeResourceFile>(ResourceFile::Type::ImageCube, FilePath(path));
}

FontResourceFile* ResourceLoader::CreateFontResourceFile(const String& path)
{
	return GetCachedResource<FontResourceFile>(ResourceFile::Type::Font, FilePath(path));
}

int ResourceLoader::Preload(Vector<String> paths, Atomic<int>& progress)
{
	if (paths.empty())
	{
		PRINT("Asked to preload an empty list of files, no files preloaded");
		return 0;
	}

	std::sort(paths.begin(), paths.end());
	Vector<String> empericalPaths = { paths.front() };
	for (auto& incomingPath : paths)
	{
		if (empericalPaths.back() != incomingPath)
		{
			empericalPaths.emplace_back(incomingPath);
		}
	}

	ThreadPool& preloadThreads = Application::GetSingleton()->getThreadPool();
	Vector<Ref<Task>> preloadTasks;

	progress = 0;
	for (auto& path : empericalPaths)
	{
		Ref<Task> loadingTask(new Task([=, &progress]() {
			CreateSomeResourceFile(path);
			progress++;
		}));
		preloadTasks.push_back(loadingTask);
	}

	// FIX: This is a workaround which saves the main thread from being blocked when 1 task is submitted
	preloadTasks.push_back(Ref<Task>(new Task([]() {})));

	preloadThreads.submit(preloadTasks);

	PRINT("Preloading " + std::to_string(paths.size()) + " resource files");
	return preloadTasks.size() - 1; // 1 less for the dummy task
}

void ResourceLoader::Unload(const Vector<String>& paths)
{
	HashMap<ResourceFile::Type, Vector<Ref<ResourceFile>>> unloads;

	for (auto& path : paths)
	{
		FilePath searchPath(path);
		searchPath = searchPath.generic_string();
		for (auto& [type, files] : s_ResourcesDataFiles)
		{
			for (auto& file : files)
			{
				if (file->getPath() == searchPath)
				{
					unloads[type].push_back(file);
				}
			}
		}
	}

	for (auto& [deletionType, deletionFiles] : unloads)
	{
		for (auto& deletionFile : deletionFiles)
	    {
			for (auto& [type, files] : s_ResourcesDataFiles)
			{
				auto& eraseIt = std::find(files.begin(), files.end(), deletionFile);
				if (eraseIt != files.end())
				{
					PRINT("Unloaded " + deletionFile->getPath().generic_string());
					files.erase(eraseIt);
				}
			}
	    }
	}
}
