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
			case ResourceFile::Type::AnimatedModel:
				result = CreateAnimatedModelResourceFile(path);
				break;
			case ResourceFile::Type::CollisionModel:
				result = CreateCollisionModelResourceFile(path);
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

CollisionModelResourceFile* ResourceLoader::CreateCollisionModelResourceFile(const String& path)
{
	return GetCachedResource<CollisionModelResourceFile>(ResourceFile::Type::CollisionModel, FilePath(path));
}

AnimatedModelResourceFile* ResourceLoader::CreateAnimatedModelResourceFile(const String& path)
{
	return GetCachedResource<AnimatedModelResourceFile>(ResourceFile::Type::AnimatedModel, FilePath(path));
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
