require("rootex/script/scripts/dialogue_node")

OneOffDialogue = class(DialogueNode)

OneOffDialogue.displayDocument = rmlui.contexts["default"]:LoadDocument("rootex/script/scripts/one_off_dialogue.rml")

function OneOffDialogue:init(text)
	self.text = text
end

function OneOffDialogue:getDocument()
	OneOffDialogue.displayDocument:GetElementById("dialogue_text").inner_rml = self.text
	return OneOffDialogue.displayDocument
end

function OneOffDialogue:handleInput(input)
	if input == DialogueInput.Next then
		return self:getNextNode()
	end
	return nil
end
