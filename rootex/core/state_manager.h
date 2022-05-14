#pragma once

#include "common/common.h"

class StateManager;

class State
{
public:
	virtual ~State() = default;

	virtual void update(StateManager& manager, float deltaMilliseconds) {};

	virtual void enter(StateManager& manager) { }
	virtual Ptr<State> exit(StateManager& manager, float deltaMilliseconds) = 0;
};

/// A state-machine-like class used to mantain and update states in classes like the PlayerController.
class StateManager
{
protected:
	Ptr<State> m_CurrentState;

	StateManager(Ptr<State>&& currentState);
	virtual ~StateManager() = default;

public:
	virtual void update(float deltaMilliseconds);

	void transition(Ptr<State> newState);
};
