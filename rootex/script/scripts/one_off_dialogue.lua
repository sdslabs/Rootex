require("rootex/script/scripts/dialogue_node")

OneOffDialogue = class(DialogueNode)

function OneOffDialogue:init(text)
	self.text = text
	self.document = rmlui.contexts["default"]:LoadDocument("rootex/script/scripts/one_off_dialogue.rml")
	self.document:Hide()
end

function OneOffDialogue:getDocument()
	self.document:GetElementById("dialogue_text").inner_rml = self.text
	return self.document
end

function OneOffDialogue:handleInput(input)
	if input == DialogueInput.Next then
		return self:getNextNode()
	end
	return nil
end
