#pragma once

#include "common/types.h"

struct Theme
{
public:
	virtual void apply() const = 0;
	virtual String getName() const = 0;
	virtual ~Theme() = default;
};