#pragma once

#include "common/types.h"

template <typename dataType>
class CustomIterator
{
protected:
	dataType* m_ptr;
	int m_index;
	bool* m_isValid;

public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = dataType;
	using difference_type = std::ptrdiff_t;
	using pointer = dataType*;
	using reference = dataType&;

	// Constructors
	CustomIterator(bool* m_isValid, dataType* ptr = nullptr)
	{
		this->m_isValid = m_isValid;
		m_ptr = ptr;
		m_index = 0;
	}
	CustomIterator(dataType* ptr = nullptr)
	{
		m_ptr = ptr;
		m_index = 0;
	}
	CustomIterator(const CustomIterator<dataType>& rawIterator) = default; // Copy constructor
	~CustomIterator() { }

	// Overloading = operator
	CustomIterator<dataType>& operator=(const CustomIterator<dataType>& rawIterator) = default;
	CustomIterator<dataType>& operator=(dataType* ptr)
	{
		m_ptr = ptr;
		return (*(this));
	}

	operator bool() const
	{
		if (m_ptr)
			return true;
		else
			return false;
	}

	bool operator==(const CustomIterator<dataType>& rawIterator) const { return (m_ptr == rawIterator.getConstPtr()); }
	bool operator!=(const CustomIterator<dataType>& rawIterator) const { return (m_ptr != rawIterator.getConstPtr()); }

	// Airthmatic Overloading
	CustomIterator<dataType> operator++()
	{
		while (!(*(this->m_isValid + m_index + 1)))
		{
			m_index++;
			++m_ptr;
		}
		m_index++;
		m_ptr++;
		return (*(this));
	}

	dataType& operator*() { return *m_ptr; }
	const dataType& operator*() const { return *m_ptr; }
	dataType* operator->() { return m_ptr; }

	dataType* getPtr() const { return m_ptr; }
	const dataType* getConstPtr() const { return m_ptr; }
};
