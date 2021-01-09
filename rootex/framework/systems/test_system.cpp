#include "test_system.h"

#include "framework/ecs_factory.h"
#include "components/test_component.h"

TestSystem::TestSystem()
    : System("TestSystem", UpdateOrder::Async, false)
{
}

void TestSystem::update(float deltaMilliseconds)
{
	ZoneScoped;
	const Vector<Component*>& testComponents = ECSFactory::GetComponents<TestComponent>();

	for (auto& testComponent : testComponents)
	{
		OS::PrintWarning("TestComponent was processed by TestSystem");
	}
}
