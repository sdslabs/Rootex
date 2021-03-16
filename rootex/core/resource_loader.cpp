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

	Vector<Ref<Task>> tasks;

	for (auto& path : paths)
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

	return tasks.size() - 1;
}

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
