local narrator = require("narrator/narrator")
local story
local ui
local currentChoice = 1
local maxChoices = 1

exports = {
	story = ""
}

function onBegin(entity)
	local book = narrator.parseFile(exports.story)
	story = narrator.initStory(book)
	story:begin()
	ui = entity:getUI()
	getDocument():Show()
end

function getDocument()
	return rmlui.contexts["default"].documents[ui:getDocumentID()]
end

function presentParagraphs(paragraphs)
	local text = ""
	for _, paragraph in ipairs(paragraphs) do
		text = text .. "<br />" .. paragraph.text
	end
	getDocument():GetElementById("paragraphs").inner_rml = text
end

function presentChoices(choices)
	local text = ""
	maxChoices = #choices
	for i, choice in ipairs(choices) do
		local selection = ""
		if currentChoice == i then
			selection = "> "
		end
		text = text .. "<br />" .. selection .. choice.text
	end
	getDocument():GetElementById("choices").inner_rml = text
end

function onUpdate(entity, delta)
	if story:canContinue() then
		local paragraphs = story:continue()
		presentParagraphs(paragraphs)
		if story:canChoose() then
			presentChoices(story:getChoices())
		end
	end

	if story:canChoose() then
		if RTX.InputManager.Get():wasPressed("Up") then
			currentChoice = currentChoice - 1
			if currentChoice < 1 then currentChoice = maxChoices end
			presentChoices(story:getChoices())
		end
		
		if RTX.InputManager.Get():wasPressed("Down") then
			currentChoice = currentChoice + 1
			if currentChoice > maxChoices then currentChoice = 1 end
			presentChoices(story:getChoices())
		end
		
		if RTX.InputManager.Get():wasPressed("Next") then
			story:choose(currentChoice)
			currentChoice = 1
		end
	else
		getDocument():Hide()
	end
end

function onEnd(entity)
end
