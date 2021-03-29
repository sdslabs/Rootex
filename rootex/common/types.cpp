#include "types.h"

Vector<String> Split(const String& s, char delim)
{
	Vector<String> result;
	std::stringstream ss(s);
	String item;

	while (std::getline(ss, item, delim))
	{
		result.push_back(item);
	}

	return result;
}
