#include "state_manager.h"

StateManager::StateManager(Ptr<State>&& currentState)
    : m_CurrentState(std::move(currentState))
{
}

void StateManager::update(float deltaMilliseconds)
{
	m_CurrentState->update(*this, deltaMilliseconds);
	if (Ptr<State> newState = std::move(m_CurrentState->exit(*this, deltaMilliseconds)))
	{
		transition(std::move(newState));
	}
}

void StateManager::transition(Ptr<State> newState)
{
	m_CurrentState = std::move(newState);
	m_CurrentState->enter(*this);
}
