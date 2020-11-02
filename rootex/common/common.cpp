#include "common.h"

Vector<String>& RootexUils::StringSplit(const String& s, char delim, Vector<String>& elems)
{
	StringStream ss(s);
	String item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}
