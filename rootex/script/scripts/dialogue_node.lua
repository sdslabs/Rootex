DialogueInput = {
	Idle = 0,
	Next = 1,
	ChoiceUp = 2,
	ChoiceDown = 3
}

DialogueNode = class()

DialogueNode.count = 0

function DialogueNode:init()
	self.id = DialogueNode.count
	self.nextNode = nil
	DialogueNode.count = DialogueNode.count + 1
end

-- If nil is returned, nothing happens, else the conversation moves on to the next dialogue.
function DialogueNode:handleInput(input)
	return nil
end

function DialogueNode:getNextNode()
	return self.nextNode
end

function DialogueNode:setNextNode(node)
	self.nextNode = node
end

function DialogueNode.Proceed(node, document, input)
	if node == nil then
		document:Hide()
		return nil
	end

	local ret = node:handleInput(input)
	if ret ~= nil then
		node:getDocument():Hide()
		local next = ret
		if next ~= nil then
			next:getDocument():Show()
		end
		return next
	end

	return node
end

DialogueBuilder = class()

function DialogueBuilder:begin(node)
	self.start = node
	self.current = self.start
	return self
end

function DialogueBuilder:add(node)
	self.current.nextNode = node
	self.current = self.current.nextNode
	return self
end

function DialogueBuilder:finish()
	return self.start
end
