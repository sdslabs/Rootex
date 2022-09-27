#pragma once

#include "common/types.h"

template <typename DataType>
class ComponentArrayIterator
{
protected:
	typename Vector<DataType>::iterator m_Itr;
	int m_Index;
	Vector<bool>* m_IsValid;

public:
	ComponentArrayIterator(Vector<bool>& isValid, typename Vector<DataType>::iterator itr)
	{
		m_IsValid = &isValid;
		m_Itr = itr;
		m_Index = 0;
	}
	ComponentArrayIterator(const ComponentArrayIterator<DataType>& rawIterator) = default; // Copy constructor
	~ComponentArrayIterator() { }

	ComponentArrayIterator<DataType>& operator=(const ComponentArrayIterator<DataType>& rawIterator) = default;

	bool operator==(const ComponentArrayIterator<DataType>& rawIterator) const { return (m_Itr == rawIterator.m_Itr); }
	bool operator!=(const ComponentArrayIterator<DataType>& rawIterator) const { return !(m_Itr == rawIterator.m_Itr); }

	ComponentArrayIterator<DataType> operator++()
	{
		while ((*m_IsValid)[m_Index + 1] == false && m_Index + 1 < (*m_IsValid).size())
		{
			m_Index++;
			m_Itr++;
		}
		m_Index++;
		m_Itr++;
		return *this;
	}

	DataType& operator*() { return *m_Itr; }
	const DataType& operator*() const { return *m_Itr; }
};
