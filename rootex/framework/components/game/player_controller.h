#pragma once

#include "common/common.h"
#include "framework/component.h"

class PlayerController : public Component
{
	DEFINE_COMPONENT(PlayerController);

public:
	PlayerController();
	~PlayerController() = default;

	bool setupData() override;
	JSON::json getJSON() const override;
	void draw() override;
};
