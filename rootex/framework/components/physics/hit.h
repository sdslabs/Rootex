#pragma once

#include "entity.h"

struct Hit
{
	Entity* thisOne;
	Entity* thatOne;

	Hit::Hit(Entity* left, Entity* right)
	    : thisOne(left)
	    , thatOne(right)
	{
	}
	Hit(const Hit&) = delete;
	~Hit() = default;
};
