#include "resource_manager.h"

HashMap<Ptr<IResourceData>, Vector<Ref<IResourceFile>>> ResourceManager::m_ResourceMap;

void ResourceManager::registerResourceDataResourceFile(Ptr<IResourceData> resourceData, Ptr<IResourceFile> resourceFile)
{
	auto it = m_ResourceMap.find(resourceData);
	if (it != m_ResourceMap.end())
	{
		m_ResourceMap[resourceData].push_back(resourceFile);
	}
	else
	{
		auto fileIt = m_ResourceMap[resourceData].find(resourceFile);
		if (fileIt != m_ResourceMap[resourceData].end())
		{
			ERR("ResourceManager: Added duplicate resource file under singular resource data");
			m_ResourceMap[resourceData].push_back(resourceFile);
		}
		else
		{
			m_ResourceMap.insert(resourceData, { resourceFile });
		}
	}
}

int ResourceManager::getActiveCount()
{
	return m_ResourceMap.size();
}

void ResourceManager::empty()
{
	for (auto& r : m_ResourceMap)
	{
		r = nullptr;
	}
	m_ResourceMap.clear();
}
