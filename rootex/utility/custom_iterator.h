#pragma once

#include "common/types.h"

template <typename dataType>
class CustomIterator
{
protected:
	typename Vector<dataType>::iterator m_itr;
	int m_index;
	Vector<bool>* m_isValid;

public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = dataType;
	using difference_type = std::ptrdiff_t;
	using pointer = dataType*;
	using reference = dataType&;

	// Constructors
	CustomIterator(Vector<bool>& isValid, typename Vector<dataType>::iterator itr)
	{
		m_isValid = &isValid;
		m_itr = itr;
		m_index = 0;
	}
	CustomIterator(typename Vector<dataType>::iterator itr)
	{
		m_itr = itr;
		m_index = 0;
	}
	CustomIterator(const CustomIterator<dataType>& rawIterator) = default; // Copy constructor
	~CustomIterator() { }

	// Overloading = operator
	CustomIterator<dataType>& operator=(const CustomIterator<dataType>& rawIterator) = default;
	CustomIterator<dataType>& operator=(typename Vector<dataType>::iterator itr)
	{
		m_itr = itr;
		return (*(this));
	}

	bool operator==(const CustomIterator<dataType>& rawIterator) const { return (m_itr == rawIterator.getConstItr()); }
	bool operator!=(const CustomIterator<dataType>& rawIterator) const { return !(m_itr == rawIterator.getConstItr()); }

	// Custom definition of ++ Overloading
	CustomIterator<dataType> operator++()
	{
		while ((*m_isValid)[m_index + 1] == false)
		{
			m_index++;
			++m_itr;
		}
		m_index++;
		m_itr++;
		return (*(this));
	}

	dataType& operator*() { return *m_itr; }
	const dataType& operator*() const { return *m_itr; }
	typename Vector<dataType>::iterator operator->() { return *m_itr; }

	typename Vector<dataType>::iterator getItr() const { return m_itr; }
	const typename Vector<dataType>::iterator getConstItr() const { return m_itr; }
};
