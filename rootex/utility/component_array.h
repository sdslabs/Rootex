#pragma once

#include "common/types.h"
#include "custom_iterator.h"

#define MAX_COMPONENT_ARRAY_arraySize 10000

template <typename component, class A = std::allocator<component>>
class ComponentArray // component array
{
	component m_data[MAX_COMPONENT_ARRAY_arraySize];
	bool m_isValid[MAX_COMPONENT_ARRAY_arraySize];
	int curr;
	size_t arraySize;

public:
	ComponentArray() //default constructor
	{
		for (int i = 0; i < MAX_COMPONENT_ARRAY_arraySize; i++)
		{
			m_isValid[i] = true;
			//m_data[i] = component();
		}
		curr = 0;
		arraySize = 0;
	}
	CustomIterator<component> begin() // begining of the iterator
	{
		int index = 0;
		while (!m_isValid[index])
		{
			index++;
		}
		return CustomIterator<component>(m_isValid, m_data + index);
	}

	CustomIterator<component> end() // end of the iterator
	{
		return CustomIterator<component>(m_data + curr);
	}

	void push_back(const component& item)
	{
		if (arraySize == MAX_COMPONENT_ARRAY_arraySize)
		{
			ERR("Component set for " + component::s_Name + " is full. Reduce component count or increase max arraySize");
		}
		for (int i = 0; i < MAX_COMPONENT_ARRAY_arraySize; i++)
		{
			if (!m_isValid[i])
			{
				m_data[i] = item;
				m_isValid[i] = true;
				return;
			}
		}
		m_data[curr] = item;
		curr++;
	}

	void emplace_back(Entity& owner, const JSON::json& componentData)
	{
		if (arraySize == MAX_COMPONENT_ARRAY_arraySize)
		{
			ERR("Component set for " + component::s_Name + " is full. Reduce component count or increase max arraySize");
		}
		component& instance = component(owner, componentData);
		owner.registerComponent(&instance);

		for (int i = 0; i < MAX_COMPONENT_ARRAY_arraySize; i++)
		{
			if (!m_isValid[i])
			{
				new (m_data + i) component(owner, componentData);
				m_isValid[i] = true;
				arraySize++;
				return;
			}
		}
		new (m_data + curr) component(owner, componentData);
		curr++;
		arraySize++;
	}

	void erase(int index)
	{
		if (index < curr)
		{
			m_isValid[index] = false;
		}
		arraySize--;
	}

	ComponentArray<component> getALL()
	{
		return *this;
	}

	size_t size() const { return arraySize; }

	bool empty() const { return arraySize == 0; }

	component front()
	{
		return *(this->begin());
	}
	component back()
	{
		return *(this->end());
	}
};
