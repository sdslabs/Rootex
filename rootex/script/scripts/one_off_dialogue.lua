require("rootex/script/scripts/dialogue_node")

OneOffDialogue = class("OneOffDialogue", DialogueNode)

function OneOffDialogue:initialize(text, exitLogic)
	self.text = text
	self.document = rmlui.contexts["default"]:LoadDocument("rootex/script/scripts/one_off_dialogue.rml")
	self.document:Hide()
	self.exitLogic = exitLogic
end

function OneOffDialogue:getDocument()
	self.document:GetElementById("dialogue_text").inner_rml = self.text
	return self.document
end

function OneOffDialogue:handleInput(input)
	if input == DialogueInput.Next then
		if self.exitLogic ~= nil then self.exitLogic(self) end
		return self:getNextNode()
	end
	return nil
end
