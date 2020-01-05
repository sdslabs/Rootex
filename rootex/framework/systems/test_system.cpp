#include "test_system.h"

#include "components/test_component.h"

void TestSystem::update(float deltaMilliseconds)
{
	const Vector<Component*> testComponents = s_Components[TestComponent::s_ID];

	for (auto& testComponent : testComponents)
	{
		OS::PrintWarning("TestComponent was processed by TestSystem");
	}
}
