require("game/assets/scripts/utility/dialogue/dialogue_node")

OneOffDialogue = class("OneOffDialogue", DialogueNode)

OneOffDialogue.static.document = rmlui.contexts["default"]:LoadDocument("game/assets/ui/dialogue/one_off_dialogue.rml")
OneOffDialogue.static.document:Hide()

function OneOffDialogue:initialize(portrait, text, exitLogic)
	DialogueNode.initialize(self, exitLogic)
	self.text = text
	self.portrait = portrait
end

function OneOffDialogue:getDocument()
	OneOffDialogue.static.document:GetElementById("dialogue_text").inner_rml = self.text
	OneOffDialogue.static.document:GetElementById("dialogue_portrait"):SetAttribute("src", "../../..://dummy/" .. self.portrait.image)
	return OneOffDialogue.static.document
end

function OneOffDialogue:handleInput(input)
	if input == DialogueInput.Next then
		return self:getNextNode()
	end
	return self
end
