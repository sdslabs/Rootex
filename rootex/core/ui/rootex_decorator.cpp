#include "rootex_decorator.h"

void RootexDecorator::UpdateAll(float deltaSeconds)
{
	for (auto& decorator : s_DecoratorInstances)
	{
		decorator->update(deltaSeconds);
	}
}

RootexDecorator::RootexDecorator()
{
	s_DecoratorInstances.push_back(this);
}

RootexDecorator::~RootexDecorator()
{
	s_DecoratorInstances.erase(std::find(s_DecoratorInstances.begin(), s_DecoratorInstances.end(), this));
}
