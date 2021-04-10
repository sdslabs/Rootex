#include "player_controller.h"

#include "core/resource_loader.h"
#include "core/event_manager.h"
#include "core/input/input_manager.h"
#include "systems/input_system.h"

DEFINE_COMPONENT(PlayerController);

PlayerController::PlayerController(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_PlayerState(new Idle())
    , m_DependencyOnAnimatedModelComponent(this)
    , m_DependencyOnCapsuleColliderComponent(this)
    , m_DependencyOnTransformComponent(this)
{
}

PlayerController::~PlayerController()
{
	if (m_PlayerState)
	{
		delete m_PlayerState;
	}
}

void PlayerController::update(float deltaMilliseconds)
{
	for (auto& [input, event] : PlayerInputEvents)
	{
		if (State* newState = m_PlayerState->handleInput(*this, input))
		{
			delete m_PlayerState;
			m_PlayerState = newState;
		}
	}
	PRINT(getAnimatedModelComponent()->getCurrentAnimationName());
	m_PlayerState->update(*this, deltaMilliseconds);
}

bool PlayerController::setupData()
{
	getAnimatedModelComponent()->setAnimatedResourceFile(ResourceLoader::CreateAnimatedModelResourceFile("game/assets/models/player/player.fbx"), {});

	InputScheme scheme;
	scheme.bools.emplace_back(InputDescription { Device::Keyboard, KeyboardButton::KeyW, PlayerInputEvents.at(PlayerInput::Forward) });
	scheme.bools.emplace_back(InputDescription { Device::Keyboard, KeyboardButton::KeyA, PlayerInputEvents.at(PlayerInput::Left) });
	scheme.bools.emplace_back(InputDescription { Device::Keyboard, KeyboardButton::KeyS, PlayerInputEvents.at(PlayerInput::Back) });
	scheme.bools.emplace_back(InputDescription { Device::Keyboard, KeyboardButton::KeyD, PlayerInputEvents.at(PlayerInput::Right) });
	InputManager::GetSingleton()->addScheme("Player", scheme);
	InputSystem::GetSingleton()->enableScheme("Player", true);

	return true;
}

State* Running::handleInput(PlayerController& p, PlayerInput input)
{
	if (!InputManager::WasPressed(PlayerInputEvents.at(input)))
	{
		exit(p);
		State* newState = new Idle();
		newState->enter(p);
		return newState;
	}
	return nullptr;
}

void Running::enter(PlayerController& p)
{
	p.getAnimatedModelComponent()->setAnimation("Armature|run");
}

State* Idle::handleInput(PlayerController& p, PlayerInput input)
{
	if (InputManager::HasPressed(PlayerInputEvents.at(input)))
	{
		exit(p);
		State* newState = new Running();
		newState->enter(p);
		return newState;
	}
	return nullptr;
}

void Idle::enter(PlayerController& p)
{
	p.getAnimatedModelComponent()->setAnimation("Armature|idle");
}

void PlayerController::draw()
{
}
