#pragma once

#include "system.h"

class SerializationSystem : public System
{
	SerializationSystem() = default;
	SerializationSystem(SerializationSystem&) = delete;
	~SerializationSystem() = default;
public:
	static SerializationSystem* GetSingleton();

	void saveAllEntities(const String& dirPath);
};
