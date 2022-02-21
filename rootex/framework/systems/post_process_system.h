#pragma once

#include "common/types.h"
#include "system.h"

#include "core/renderer/post_processor.h"

/// Handles all the custom post processes
class PostProcessSystem : public System
{
	PostProcessor m_Processor;
	PostProcessingDetails m_PostProcessingDetails;

	PostProcessSystem();
	PostProcessSystem(const PostProcessSystem&) = delete;
	~PostProcessSystem() = default;

public:
	static PostProcessSystem* GetSingleton();

	void update(float deltaMilliseconds) override;

	void addCustomPostProcessing(const String& path);
};
