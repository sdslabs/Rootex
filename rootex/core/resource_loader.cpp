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

ResourceFile* ResourceLoader::CreateResourceFile(const ResourceFile::Type& type, const String& path)
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
	default:
		break;
	}

	return nullptr;
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

int ResourceLoader::Preload(ResourceCollection paths, Atomic<int>& progress)
{
	progress = 0;
	if (paths.empty())
	{
		PRINT("Asked to preload an empty list of files, no files preloaded");
		return 0;
	}

	std::sort(paths.begin(), paths.end(), [](const Pair<ResourceFile::Type, String>& a, const Pair<ResourceFile::Type, String>& b) {
		return a.second < a.second;
	});

	ResourceCollection empericalPaths = { paths.front() };
	for (auto& incomingPath : paths)
	{
		if (empericalPaths.back() != incomingPath)
		{
			empericalPaths.emplace_back(incomingPath);
		}
	}

	ThreadPool& preloadThreads = Application::GetSingleton()->getThreadPool();
	Vector<Ref<Task>> preloadTasks;

	for (auto& path : empericalPaths)
	{
		Ref<Task> loadingTask(new Task([=, &progress]() {
			CreateResourceFile((ResourceFile::Type)path.first, path.second);
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

void ResourceLoader::Unload(const Vector<Pair<ResourceFile::Type, String>>& paths)
{
	HashMap<ResourceFile::Type, Vector<Ref<ResourceFile>>> unloads;

	for (auto& path : paths)
	{
		FilePath searchPath(path.second);
		searchPath = searchPath.generic_string();
		for (auto& file : s_ResourcesDataFiles.at(path.first))
		{
			String filePathString = file->getPath().generic_string();
			if (file->getPath() == searchPath && filePathString.find("rootex/assets/") == String::npos)
			{
				unloads[path.first].push_back(file);
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
