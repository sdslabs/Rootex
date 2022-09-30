#pragma once

#include "common/types.h"
#include "component_array_iterator.h"

#define MAX_COMPONENT_ARRAY_SIZE 10000

template <typename Component, class A = std::allocator<Component>>
class ComponentArray
{
private:
	Vector<Component> m_Data;
	Vector<bool> m_IsValid;
	size_t m_Curr;
	size_t m_ArraySize;

public:
	ComponentArray()
	{
		m_Data.reserve(MAX_COMPONENT_ARRAY_SIZE);
		m_IsValid.reserve(MAX_COMPONENT_ARRAY_SIZE);
		for (int i = 0; i < MAX_COMPONENT_ARRAY_SIZE; i++)
		{
			m_IsValid.push_back(true);
		}
		m_Curr = 0;
		m_ArraySize = 0;
	}

	ComponentArrayIterator<Component> begin()
	{
		int index = 0;
		while (!m_IsValid[index])
		{
			index++;
		}
		return ComponentArrayIterator<Component>(m_IsValid, m_Data.begin() + index);
	}

	ComponentArrayIterator<Component> end() { return ComponentArrayIterator<Component>(m_IsValid, m_Data.begin() + m_Curr); }

	void push_back(const Component& item)
	{
		if (m_ArraySize == MAX_COMPONENT_ARRAY_SIZE)
		{
			ERR("Component set for " + Component::s_Name + " is full. Reduce component count or increase MAX_COMPONENT_ARRAY_SIZE");
		}
		for (int i = 0; i < m_Curr; i++)
		{
			if (!m_IsValid[i])
			{
				m_Data[i] = item;
				m_IsValid[i] = true;
				m_ArraySize++;
				return;
			}
		}
		m_Data[m_Curr] = item;
		m_Curr++;
		m_ArraySize++;
	}

	void emplace_back(Entity& owner, const JSON::json& componentData)
	{
		if (m_ArraySize == MAX_COMPONENT_ARRAY_SIZE)
		{
			ERR("Component set for " + Component::s_Name + " is full. Reduce component count or increase max MAX_COMPONENT_ARRAY_SIZE");
		}

		for (int i = 0; i < m_Curr; i++)
		{
			if (!m_IsValid[i])
			{
				new (&m_Data[i]) Component(owner, componentData); //Create a new component at m_data[i]
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
				m_ArraySize--;
				return true;
			}
		}
		return false;
	}

	Component& ComponentArray::operator[](int index)
	{
		if (index >= m_ArraySize)
		{
			ERR("Array index out of bound");
		}

		int actualIndex = 0;
		int i = 0;
		for (i = 0; i < m_Curr; i++)
		{
			if (m_IsValid[i])
			{
				actualIndex++;
			}
			if (actualIndex == index)
			{
				break;
			}
		}
		return m_Data[i];
	}

	size_t size() const { return m_ArraySize; }

	bool empty() const { return m_ArraySize == 0; }

	Component front() { return *begin(); }

	Component back() { return *end(); }
};
