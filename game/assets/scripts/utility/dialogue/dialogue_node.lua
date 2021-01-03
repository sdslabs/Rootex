require("game/assets/scripts/utility/common")

DialogueInput = {
	Idle = 0,
	Next = 1,
	ChoiceUp = 2,
	ChoiceDown = 3
}

Portrait = class("Portrait")

function Portrait:initialize(imagePath, width, height)
	self.image = imagePath
	self.width = width
	self.height = height
end

DialogueNode = class("DialogueNode")

DialogueNode.static.count = 0
DialogueNode.static.stack = Stack()

--[[ 
Exit logic is an optional function which user can run logic in when the node progresses. 
It should return false in case conversation needs to stop.
--]]
function DialogueNode:initialize(exitLogic)
	self.id = DialogueNode.static.count
	self.nextNode = nil
	self.exitLogic = exitLogic
	self.template = ""
	DialogueNode.static.count = DialogueNode.static.count + 1
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
		document:Hide()
		return node
	end

	node:getDocument():Hide()

	local continue = true
	-- If exitLogic exists and returns false, stop the conversation
	if node.exitLogic ~= nil then
		continue = node.exitLogic(node)
	end

	-- Decide if we should still display the next node
	local next = node:handleInput(input)
	if next ~= nil and continue ~= false then
		next:getDocument():Show()
		return next
	end

	-- Return to the dialogue stack if present
	local topDialogue = DialogueNode.static.stack:pop()
	if topDialogue ~= nil then
		topDialogue:getDocument():Show()
		return topDialogue
	end

	return nil
end

DialogueBuilder = class("DialogueBuilder")

function DialogueBuilder:add(node)
	if self.current == nil then
		self.start = node
		self.current = self.start
	end

	self.current.template = self.currentTemplate
	self.current.nextNode = node
	self.current = self.current.nextNode
	return self
end

function DialogueBuilder:addTemplate(template)
	self.currentTemplate = template
	return self
end

function DialogueBuilder:finish()
	return self.start
end
