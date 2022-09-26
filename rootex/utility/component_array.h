#pragma once

#include "common/types.h"
#include "custom_iterator.h"

#define MAX_COMPONENT_ARRAY_SIZE 10000

template <typename component, class A = std::allocator<component>>
class ComponentArray // component array
{
private:
	Vector<component> m_Data;
	Vector<bool> m_IsValid;
	int m_Curr;
	size_t m_ArraySize;

public:
	ComponentArray() //default constructor
	{
		m_Data.reserve(MAX_COMPONENT_ARRAY_SIZE);
		m_IsValid.reserve(MAX_COMPONENT_ARRAY_SIZE);
		for (int i = 0; i < MAX_COMPONENT_ARRAY_SIZE; i++)
		{
			m_IsValid.push_back(true);
			//m_Data[i] = component();
		}
		m_Curr = 0;
		m_ArraySize = 0;
	}

	CustomIterator<component> begin() // begining of the iterator
	{
		int index = 0;
		while (!m_IsValid[index])
		{
			index++;
		}
		return CustomIterator<component>(m_IsValid, m_Data.begin() + index);
	}

	CustomIterator<component> end() // end of the iterator
	{
		return CustomIterator<component>(m_Data.begin() + m_Curr);
	}

	void push_back(const component& item)
	{
		if (m_ArraySize == MAX_COMPONENT_ARRAY_SIZE)
		{
			ERR("Component set for " + component::s_Name + " is full. Reduce component count or increase max m_ArraySize");
		}
		for (int i = 0; i < MAX_COMPONENT_ARRAY_SIZE; i++)
		{
			if (!m_IsValid[i])
			{
				m_Data[i] = item;
				m_IsValid[i] = true;
				return;
			}
		}
		m_Data[m_Curr] = item;
		m_Curr++;
	}

	void emplace_back(Entity& owner, const JSON::json& componentData)
	{
		if (m_ArraySize == MAX_COMPONENT_ARRAY_SIZE)
		{
			ERR("Component set for " + component::s_Name + " is full. Reduce component count or increase max m_ArraySize");
		}

		for (int i = 0; i < m_Curr; i++)
		{
			if (!m_IsValid[i])
			{
				new (&m_Data[i]) component(owner, componentData); //Create a new component at m_data[i]
				owner.registerComponent(&m_Data[i]);
				m_IsValid[i] = true;
				m_ArraySize++;
				return;
			}
		}
		m_Data.emplace_back(owner, componentData);
		owner.registerComponent(&m_Data[m_Curr]);
		m_Curr++;
		m_ArraySize++;
	}

	bool erase(Entity& entity)
	{
		for (int i = 0; i <= m_Curr; i++)
		{
			if (m_IsValid[i] && (m_Data[i].getOwner().getID() == entity.getID()))
			{
				m_IsValid[i] = false;
				return true;
			}
		}
		return false;
	}

	ComponentArray<component> getALL()
	{
		return *this;
	}

	size_t size() const { return m_ArraySize; }

	bool empty() const { return m_ArraySize == 0; }

	component front()
	{
		return *(this->begin());
	}

	component back()
	{
		return *(this->end());
	}
};
