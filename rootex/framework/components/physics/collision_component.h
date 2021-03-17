#pragma once

#include "common/types.h"
#include "component.h"
#include "hit.h"

#include "btBulletCollisionCommon.h"

enum class CollisionMask : unsigned int
{
	None = 0,
	Player = 1 << 0,
	Enemy = 1 << 1,
	Architecture = 1 << 2,
	TriggerVolume = 1 << 3,
	Other = 1 << 4,
	All = Player | Enemy | Architecture | TriggerVolume | Other
};

class CollisionComponent : public Component
{
protected:
	unsigned int m_CollisionGroup;
	unsigned int m_CollisionMask;

public:
	CollisionComponent(int collisionGroup, int collisionMask);
	virtual ~CollisionComponent() = default;

	virtual void handleHit(Hit* h) = 0;

	JSON::json getJSON() const override;
	void draw() override;
	void displayCollisionLayers(unsigned int& collision);
};
