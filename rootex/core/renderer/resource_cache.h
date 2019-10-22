#pragma once

#include "common/types.h"

class IResourceCache
{
public:
	virtual Weak<IResourceFile> GetResource(Weak<IResourc> resource);
}
