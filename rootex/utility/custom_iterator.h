#pragma once

#include "common/types.h"

template <typename dataType>
class CustomArrayIterator
{
protected:
	typename Vector<dataType>::iterator m_Itr;
	int m_Index;
	Vector<bool>* m_IsValid;

public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = dataType;
	using difference_type = std::ptrdiff_t;
	using pointer = dataType*;
	using reference = dataType&;

	// Constructors
	CustomArrayIterator(Vector<bool>& isValid, typename Vector<dataType>::iterator itr)
	{
		m_IsValid = &isValid;
		m_Itr = itr;
		m_Index = 0;
	}
	CustomArrayIterator(typename Vector<dataType>::iterator itr)
	{
		m_Itr = itr;
		m_Index = 0;
	}
	CustomArrayIterator(const CustomArrayIterator<dataType>& rawIterator) = default; // Copy constructor
	~CustomArrayIterator() { }

	// Overloading = operator
	CustomArrayIterator<dataType>& operator=(const CustomArrayIterator<dataType>& rawIterator) = default;
	CustomArrayIterator<dataType>& operator=(typename Vector<dataType>::iterator itr)
	{
		m_Itr = itr;
		return *this;
	}

	bool operator==(const CustomArrayIterator<dataType>& rawIterator) const { return (m_Itr == rawIterator.getConstItr()); }
	bool operator!=(const CustomArrayIterator<dataType>& rawIterator) const { return !(m_Itr == rawIterator.getConstItr()); }

	// Custom definition of ++ Overloading
	CustomArrayIterator<dataType> operator++()
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

	dataType& operator*() { return *m_Itr; }
	const dataType& operator*() const { return *m_Itr; }
	typename Vector<dataType>::iterator operator->() { return *m_Itr; }

	typename Vector<dataType>::iterator getItr() const { return m_Itr; }
	const typename Vector<dataType>::iterator getConstItr() const { return m_Itr; }
};
