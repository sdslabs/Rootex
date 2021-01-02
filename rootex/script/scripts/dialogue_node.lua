DialogueInput = {
	Idle = 0,
	Next = 1,
	ChoiceUp = 2,
	ChoiceDown = 3
}

DialogueNode = class("DialogueNode")

DialogueNode.count = 0

--[[ 
Exit logic is an optional function which user can run logic in when the node progresses. 
It should return false in case conversation needs to stop.
--]]
function DialogueNode:initialize(exitLogic)
	self.id = DialogueNode.count
	self.nextNode = nil
	self.exitLogic = exitLogic
	DialogueNode.count = DialogueNode.count + 1
end

-- Returns the node which the conversation should jump to next, based on the input.
function DialogueNode:handleInput(input)
	return nil
end

function DialogueNode:getNextNode()
	return self.nextNode
end

function DialogueNode:setNextNode(node)
	self.nextNode = node
end

-- Returns the next node to display if conversation is remaining, else returns the current node itself
function DialogueNode.static:Proceed(node, document, input)
	if node == nil then
		document.context:UnloadDocument(document)
		return node
	end

	local next = node:handleInput(input)
	node:getDocument():Hide()

	local continue = true
	-- If exitLogic exists and returns false, stop the conversation
	if node.exitLogic ~= nil then
		continue = node.exitLogic(node)
	end

	-- Decide if we should still display the next node
	if next ~= nil and continue ~= false then
		next:getDocument():Show()
		return next
	end

	return node
end

DialogueBuilder = class("DialogueBuilder")

function DialogueBuilder:add(node)
	if self.current == nil then
		self.start = node
		self.current = self.start
	end

	self.current.nextNode = node
	self.current = self.current.nextNode
	return self
end

function DialogueBuilder:finish()
	return self.start
end
