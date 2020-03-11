#pragma once

#include "system.h"

class SerializationSystem : public System
{
public:
	SerializationSystem() = default;
	SerializationSystem(SerializationSystem&) = delete;
	~SerializationSystem() = default;

	void saveAllComponents(const String& dirPath, const String& projectName);
};
