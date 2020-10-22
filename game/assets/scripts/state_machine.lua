State = class()
function State:init(name, desc)
	self.m_Name = name
	self.m_Desc = desc
	self.m_NextState = nil
end

function State:connect(state)
	self.m_NextState = state
	return state
end

function State:getNextState()
	return self.m_NextState
end

function State:getName()
	return self.m_Name
end

function State:getDesc()
	return self.m_Desc
end

StateMachine = class()
function StateMachine:init()
	local state0 = State("State0", "I think we are not supposed to be here")
	local state1 = State("State1", "I think I heard something!")
	local state2 = State("State2", "Did you hear that?")
	local state3 = State("State3", "Come on let's go")

	state0:connect(state1):connect(state2):connect(state3)

	self.m_CurrentState = state0
end

function StateMachine:run()
	if self.m_CurrentState ~= nil then
		print(self.m_CurrentState:getName())
		print(self.m_CurrentState:getDesc())
		
		if RTX.InputManager.Get():wasPressed("A") then
			self.m_CurrentState = self.m_CurrentState:getNextState()
		end
	end
end

function StateMachine:change()
	if self.m_CurrentState ~= nil then
		print(self.m_CurrentState:getName())
		print(self.m_CurrentState:getDesc())
		
		self.m_CurrentState = self.m_CurrentState:getNextState()
	end
	return self.m_CurrentState	
end
