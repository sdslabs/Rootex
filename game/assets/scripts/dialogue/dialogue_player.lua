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
	if self.story:canContinue() then
		local paragraphs = self.story:continue()
		self:presentParagraphs(paragraphs)
		if self.story:canChoose() then
			self:presentChoices(self.story:getChoices())
		end
	end

	if self.story:canChoose() then
		if RTX.Input.WasPressed("Up") then
			self.currentChoice = currentChoice - 1
			if self.currentChoice < 1 then self.currentChoice = self.maxChoices end
			self:presentChoices(self.story:getChoices())
		end
		
		if RTX.Input.WasPressed("Down") then
			self.currentChoice = self.currentChoice + 1
			if self.currentChoice > self.maxChoices then self.currentChoice = 1 end
			self:presentChoices(self.story:getChoices())
		end
		
		if RTX.Input.WasPressed("Next") then
			self.story:choose(self.currentChoice)
			self.currentChoice = 1
		end
	else
		self:getDocument():Hide()
	end
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
		if self.currentChoice == i then
			selection = "> "
		end
		text = text .. "<br />" .. selection .. choice.text
	end
	self:getDocument():GetElementById("choices").inner_rml = text
end

function DialoguePlayer:onEnd(entity)
end

return DialoguePlayer