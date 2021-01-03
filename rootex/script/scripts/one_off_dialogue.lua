require("rootex/script/scripts/dialogue_node")

OneOffDialogue = class("OneOffDialogue", DialogueNode)

OneOffDialogue.static.document = rmlui.contexts["default"]:LoadDocument("rootex/script/scripts/one_off_dialogue.rml")
OneOffDialogue.static.document:Hide()

function OneOffDialogue:initialize(portrait, text, exitLogic)
	DialogueNode.initialize(self, exitLogic)
	self.text = text
	self.portrait = portrait
end

function OneOffDialogue:getDocument()
	OneOffDialogue.static.document:GetElementById("dialogue_text").inner_rml = self.text
	local portraitFormat = [[<img src="../../..://dummy/{src}" width={width}em height={height}em />]]
	portraitFormat = portraitFormat
	:gsub("{src}", self.portrait.image)
	:gsub("{width}", self.portrait.width)
	:gsub("{height}", self.portrait.height)
	OneOffDialogue.static.document:GetElementById("dialogue_portrait").inner_rml = portraitFormat
	return OneOffDialogue.static.document
end

function OneOffDialogue:handleInput(input)
	if input == DialogueInput.Next then
		return self:getNextNode()
	end
	return self
end
