#pragma once

#include "common/common.h"
#include "framework/component.h"
#include "framework/components/space/transform_component.h"
#include "framework/components/visual/model/animated_model_component.h"
#include "framework/components/physics/capsule_collider_component.h"

class State;

class PlayerController : public Component
{
	COMPONENT(PlayerController, Category::Game);
	DEPENDS_ON(TransformComponent);
	DEPENDS_ON(AnimatedModelComponent);
	DEPENDS_ON(CapsuleColliderComponent);

	State* m_PlayerState = nullptr;

public:
	PlayerController(Entity& owner, const JSON::json& data);
	~PlayerController();

	void update(float deltaMilliseconds);

	bool setupData() override;
	void draw() override;
};

DECLARE_COMPONENT(PlayerController);

enum class PlayerInput
{
	Forward,
	Back,
	Right,
	Left
};

inline const HashMap<PlayerInput, Event::Type> PlayerInputEvents = {
	{ PlayerInput::Forward, "PlayerForward" },
	{ PlayerInput::Back, "PlayerBack" },
	{ PlayerInput::Right, "PlayerRight" },
	{ PlayerInput::Left, "PlayerLeft" },
};

class State
{
public:
	virtual ~State() = default;
	virtual State* handleInput(PlayerController& p, PlayerInput input) { return nullptr; };
	virtual void update(PlayerController& p, float deltaMilliseconds) {};
	virtual void enter(PlayerController& p) {};
	virtual void exit(PlayerController& p) {};
};

class Running : public State
{
public:
	State* handleInput(PlayerController& p, PlayerInput input) override;
	void enter(PlayerController& p) override;
};

class Idle : public State
{
public:
	State* handleInput(PlayerController& p, PlayerInput input) override;
	void enter(PlayerController& p) override;
};
