#pragma once

#include "system.h"

/// Implements process of serialization for the entities.
class SerializationSystem : public System
{
	SerializationSystem();
	SerializationSystem(SerializationSystem&) = delete;
	~SerializationSystem() = default;
public:
	static SerializationSystem* GetSingleton();

	void saveAllEntities(const String& dirPath);
};
