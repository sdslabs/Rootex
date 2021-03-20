DialoguePlayer = class("DialoguePlayer")

function DialoguePlayer:initialize(entity)
    self.exports = {
		story = ""
	}
end

function DialoguePlayer:begin(entity)
	self.narrator = require("narrator/narrator")
	local book = self.narrator.parseFile(self.exports.story)
	self.story = self.narrator.initStory(book)
	self.story:begin()
	self.ui = entity:getUI()
	self.currentChoice = 1
	self.maxChoices = 1
	self:getDocument():Show()
	print("reahed")
end

function DialoguePlayer:update(entity, delta)
end


function DialoguePlayer:getDocument()
	return rmlui.contexts["default"].documents[self.ui:getDocumentID()]
end

function DialoguePlayer:presentParagraphs(paragraphs)
	local text = ""
	for _, paragraph in ipairs(paragraphs) do
		text = text .. "<br />" .. paragraph.text
	end
	self:getDocument():GetElementById("paragraphs").inner_rml = text
end

function DialoguePlayer:presentChoices(choices)
	local text = ""
	self.maxChoices = #choices
	for i, choice in ipairs(choices) do
		local selection = ""
		if currentChoice == i then
			selection = "> "
		end
		text = text .. "<br />" .. selection .. choice.text
	end
	self:getDocument():GetElementById("choices").inner_rml = text
end

function DialoguePlayer:onUpdate(entity, delta)
	if self.story:canContinue() then
		local paragraphs = self.story:continue()
		presentParagraphs(paragraphs)
		if self.story:canChoose() then
			presentChoices(self.story:getChoices())
		end
	end

	if self.story:canChoose() then
		if RTX.InputManager.Get():wasPressed("Up") then
			currentChoice = currentChoice - 1
			if currentChoice < 1 then currentChoice = maxChoices end
			presentChoices(self.story:getChoices())
		end
		
		if RTX.InputManager.Get():wasPressed("Down") then
			currentChoice = currentChoice + 1
			if currentChoice > maxChoices then currentChoice = 1 end
			presentChoices(self.story:getChoices())
		end
		
		if RTX.InputManager.Get():wasPressed("Next") then
			self.story:choose(currentChoice)
			currentChoice = 1
		end
	else
		self:getDocument():Hide()
	end
end

function DialoguePlayer:onEnd(entity)
end

return DialoguePlayer