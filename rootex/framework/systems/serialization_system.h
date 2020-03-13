#pragma once

#include "system.h"

class SerializationSystem : public System
{
public:
	SerializationSystem() = default;
	SerializationSystem(SerializationSystem&) = delete;
	~SerializationSystem() = default;

	void saveAllEntities(const String& dirPath);
};
