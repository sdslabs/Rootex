require("rootex/script/scripts/dialogue_node")

OneOffDialogue = class("OneOffDialogue", DialogueNode)

OneOffDialogue.static.document = rmlui.contexts["default"]:LoadDocument("rootex/script/scripts/one_off_dialogue.rml")
OneOffDialogue.static.document:Hide()

function OneOffDialogue:initialize(text, exitLogic)
	DialogueNode.initialize(self, exitLogic)
	self.text = text
end

function OneOffDialogue:getDocument()
	OneOffDialogue.static.document:GetElementById("dialogue_text").inner_rml = self.text
	return OneOffDialogue.static.document
end

function OneOffDialogue:handleInput(input)
	if input == DialogueInput.Next then
		return self:getNextNode()
	end
	return self
end
