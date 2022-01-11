#include "system.h"

Vector<Vector<System*>> System::s_Systems;

System::System(const String& name, const UpdateOrder& order, bool isGameplay)
    : m_SystemName(name)
    , m_UpdateOrder(order)
{
	if ((int)order >= s_Systems.size())
	{
		s_Systems.resize((int)order + 1, {});
	}

	s_Systems[(int)order].push_back(this);
	setActive(isGameplay);
}

System::~System()
{
	const int updateOrderInt = (int)m_UpdateOrder;
	auto& findIt = std::find(s_Systems[updateOrderInt].begin(), s_Systems[updateOrderInt].end(), this);
	if (findIt != s_Systems[updateOrderInt].end())
	{
		s_Systems[updateOrderInt].erase(findIt);
	}
}

bool System::initialize(const JSON::json& systemData)
{
	PRINT("On demand initialization skipped for: " + m_SystemName);
	return true;
}

void System::setConfig(const SceneSettings& sceneSettings)
{
}

void System::begin()
{
}

void System::update(float deltaMilliseconds)
{
}

void System::end()
{
}

void System::setActive(bool enabled)
{
	m_IsActive = enabled;
}

void System::pause()
{
	for (auto s : GetSystems())
	{
		for (auto t : s)
		{
			t->m_Pause = true;
		}
	}
}
void System::unpause()
{
	for (auto s : GetSystems())
	{
		for (auto t : s)
		{
			t->m_Pause = false;
		}
	}
}

void System::draw()
{
	ImGui::Columns(2);

	ImGui::Text("Name");
	ImGui::NextColumn();
	ImGui::Text("%s", m_SystemName.c_str());
	ImGui::NextColumn();

	ImGui::Columns(1);
}
