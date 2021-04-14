#include "player_controller.h"

#include "core/resource_loader.h"
#include "core/event_manager.h"
#include "core/input/input_manager.h"
#include "systems/input_system.h"

DEFINE_COMPONENT(PlayerController);

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
	{ PlayerInput::Left, "PlayerLeft" }
};

class PlayerState : public State
{
protected:
	PlayerController& m_Player;

	PlayerState(PlayerController& p)
	    : m_Player(p)
	{
	}

	bool isForwardPressed() const { return InputManager::IsPressed(PlayerInputEvents.at(PlayerInput::Forward)); }
	bool isForwardHasPressed() const { return InputManager::HasPressed(PlayerInputEvents.at(PlayerInput::Forward)); }
	bool isForwardReleased() const { return InputManager::WasPressed(PlayerInputEvents.at(PlayerInput::Forward)); }

	bool isBackPressed() const { return InputManager::IsPressed(PlayerInputEvents.at(PlayerInput::Back)); }
	bool isBackHasPressed() const { return InputManager::HasPressed(PlayerInputEvents.at(PlayerInput::Back)); }
	bool isBackReleased() const { return InputManager::WasPressed(PlayerInputEvents.at(PlayerInput::Back)); }

	bool isLeftPressed() const { return InputManager::IsPressed(PlayerInputEvents.at(PlayerInput::Left)); }
	bool isLeftHasPressed() const { return InputManager::HasPressed(PlayerInputEvents.at(PlayerInput::Left)); }
	bool isLeftReleased() const { return InputManager::WasPressed(PlayerInputEvents.at(PlayerInput::Left)); }

	bool isRightPressed() const { return InputManager::IsPressed(PlayerInputEvents.at(PlayerInput::Right)); }
	bool isRightHasPressed() const { return InputManager::HasPressed(PlayerInputEvents.at(PlayerInput::Right)); }
	bool isRightReleased() const { return InputManager::WasPressed(PlayerInputEvents.at(PlayerInput::Right)); }

	bool isAnyPressed() const { return isForwardPressed() || isBackPressed() || isLeftPressed() || isRightPressed(); }
	bool isAnyHasPressed() const { return isForwardHasPressed() || isBackHasPressed() || isLeftHasPressed() || isRightHasPressed(); }
	bool isAnyReleased() const { return isForwardReleased() || isBackReleased() || isLeftReleased() || isRightReleased(); }
};

class ForwardRunState : public PlayerState
{
public:
	ForwardRunState(PlayerController& p);
	void update(StateManager& manager, float deltaMilliseconds) override;
	void enter(StateManager& manager) override;
	Ptr<State> exit(StateManager& manager, float deltaMilliseconds) override;
};

class IdleState : public PlayerState
{
public:
	IdleState(PlayerController& p);
	void enter(StateManager& manager) override;
	Ptr<State> exit(StateManager& manager, float deltaMilliseconds) override;
};

class PlayerStateManager : public StateManager
{
public:
	PlayerStateManager(PlayerController& player)
	    : StateManager(std::make_unique<IdleState>(player))
	{
	}
	~PlayerStateManager() = default;

	void update(float deltaMilliseconds) override
	{
		StateManager::update(deltaMilliseconds);
	}
};

ForwardRunState::ForwardRunState(PlayerController& p)
    : PlayerState(p)
{
}

void ForwardRunState::update(StateManager& manager, float deltaMilliseconds)
{
	if (isForwardPressed())
	{
		Vector3 front;
		m_Player.getTransformComponent()->getAbsoluteTransform().Forward().Normalize(front);
		m_Player.m_Velocity += front * m_Player.m_SpeedModifier * m_Player.m_Acceleration * deltaMilliseconds * MS_TO_S;
	}
	else
	{
		Vector3 direction;
		m_Player.m_Velocity.Normalize(direction);
		m_Player.m_Velocity -= direction * m_Player.m_StoppingPower * deltaMilliseconds * MS_TO_S;
	}

	if (m_Player.m_Velocity.Length() > m_Player.m_MaxRunSpeed)
	{
		Vector3 direction = m_Player.m_Velocity;
		direction.Normalize();
		m_Player.m_Velocity = direction * m_Player.m_MaxRunSpeed;
	}

	m_Player.getCapsuleColliderComponent()->setVelocity(m_Player.m_Velocity);
	m_Player.getAnimatedModelComponent()->setSpeedMultiplier(
	    m_Player.m_Velocity.Length() / m_Player.m_MaxRunSpeed);
};

void ForwardRunState::enter(StateManager& manager)
{
	m_Player.getAnimatedModelComponent()->transition(m_Player.m_RunAnimation, 1.0f);
}

Ptr<State> ForwardRunState::exit(StateManager& manager, float deltaMilliseconds)
{
	if (isForwardPressed())
	{
		return nullptr;
	}

	if (m_Player.getCapsuleColliderComponent()->getVelocity().Length() < m_Player.m_IdleThreshold)
	{
		return std::make_unique<IdleState>(m_Player);
	}

	return nullptr;
}

IdleState::IdleState(PlayerController& p)
    : PlayerState(p)
{
}

void IdleState::enter(StateManager& manager)
{
	m_Player.m_Velocity = Vector3::Zero;
	m_Player.getAnimatedModelComponent()->transition(m_Player.m_IdleAnimation, 1.0f);
}

Ptr<State> IdleState::exit(StateManager& manager, float deltaMilliseconds)
{
	if (isForwardPressed())
	{
		return std::make_unique<ForwardRunState>(m_Player);
	}
	return nullptr;
}

PlayerController::PlayerController(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_WalkAnimation(data.value("walk", ""))
    , m_RunAnimation(data.value("run", ""))
    , m_IdleAnimation(data.value("idle", ""))
    , m_TurnLeftAnimation(data.value("turnLeft", ""))
    , m_TurnRightAnimation(data.value("turnRight", ""))
    , m_PlayerAnimation(data.value("playerAnimation", "game/assets/models/player/player.fbx"))
    , m_MaxWalkSpeed(data.value("maxWalkSpeed", 0.5f))
    , m_MaxRunSpeed(data.value("maxRunSpeed", 1.0f))
    , m_SpeedModifier(data.value("speedModifier", 1.0f))
    , m_StoppingPower(data.value("stoppingPower", 1.0f))
    , m_IdleThreshold(data.value("idleThreshold", 0.1f))
    , m_Acceleration(data.value("acceleration", 1.0f))
    , m_StateManager(std::make_shared<PlayerStateManager>(*this))
    , m_DependencyOnAnimatedModelComponent(this)
    , m_DependencyOnCapsuleColliderComponent(this)
    , m_DependencyOnTransformComponent(this)
{
}

void PlayerController::update(float deltaMilliseconds)
{
	m_StateManager->update(deltaMilliseconds);
}

bool PlayerController::setupData()
{
	getAnimatedModelComponent()->setAnimatedResourceFile(ResourceLoader::CreateAnimatedModelResourceFile(m_PlayerAnimation), {});
	getCapsuleColliderComponent()->setMoveable(true);

	InputScheme scheme;
	scheme.bools.push_back(InputDescription { Device::Keyboard, KeyboardButton::KeyW, PlayerInputEvents.at(PlayerInput::Forward) });
	scheme.bools.push_back(InputDescription { Device::Keyboard, KeyboardButton::KeyA, PlayerInputEvents.at(PlayerInput::Left) });
	scheme.bools.push_back(InputDescription { Device::Keyboard, KeyboardButton::KeyS, PlayerInputEvents.at(PlayerInput::Back) });
	scheme.bools.push_back(InputDescription { Device::Keyboard, KeyboardButton::KeyD, PlayerInputEvents.at(PlayerInput::Right) });
	InputManager::GetSingleton()->addScheme("Player", scheme);
	InputSystem::GetSingleton()->enableScheme("Player", true);

	return true;
}

JSON::json PlayerController::getJSON() const
{
	JSON::json j;

	j["playerAnimation"] = m_PlayerAnimation;
	j["walk"] = m_WalkAnimation;
	j["run"] = m_RunAnimation;
	j["idle"] = m_IdleAnimation;
	j["turnLeft"] = m_TurnLeftAnimation;
	j["turnRight"] = m_TurnRightAnimation;
	j["maxWalkSpeed"] = m_MaxWalkSpeed;
	j["maxRunSpeed"] = m_MaxRunSpeed;
	j["acceleration"] = m_Acceleration;
	j["speedModifier"] = m_SpeedModifier;
	j["stoppingPower"] = m_StoppingPower;
	j["idleThreshold"] = m_IdleThreshold;

	return j;
}

void PlayerController::draw()
{
	drawAnimation("Idle", m_IdleAnimation);
	drawAnimation("Walk", m_WalkAnimation);
	drawAnimation("Run", m_RunAnimation);
	drawAnimation("Turn Left", m_TurnLeftAnimation);
	drawAnimation("Turn Right", m_TurnRightAnimation);

	ImGui::DragFloat("Idle Threshold", &m_IdleThreshold, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Acceleration", &m_Acceleration, 0.01f, 0.0f, 5.0f);
	ImGui::DragFloat("Max Walk Speed", &m_MaxWalkSpeed, 0.01f, 0.001f, 100.0f);
	ImGui::DragFloat("Max Run Speed", &m_MaxRunSpeed, 0.01f, 0.001f, 100.0f);
	ImGui::DragFloat("Speed Modifier", &m_SpeedModifier, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("Stopping Power", &m_StoppingPower, 0.01f, 0.0f, 1.0f);
}

void PlayerController::drawAnimation(const char* animation, String& editing)
{
	if (ImGui::BeginCombo(animation, editing.c_str()))
	{
		for (auto& [animationName, animationData] : getAnimatedModelComponent()->getAnimatedResourceFile()->getAnimations())
		{
			if (ImGui::MenuItem(animationName.c_str(), nullptr))
			{
				editing = animationName;
			}
		}
		ImGui::EndCombo();
	}
}
