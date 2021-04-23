#pragma once

#include "common/common.h"

#include "RmlUi/Core/Decorator.h"

class RootexDecorator : public Rml::Decorator
{
	static inline Vector<RootexDecorator*> s_DecoratorInstances;

public:
	static void UpdateAll(float deltaSeconds);

	RootexDecorator();
	virtual ~RootexDecorator();

	virtual void update(float deltaSeconds) = 0;
};
