DialoguePlayer = class("DialoguePlayer")

function DialoguePlayer:initialize(entity)
    self.exports = {
		story = "",
		up = "Up",
		down = "Down",
		next = "Next"
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
	self.currentActor = ""
	if self.story.variables["defaultColor"] then
		self.defaultColor = self.story.variables["defaultColor"]
	else 
		self.defaultColor = "white"
	end
	if self.story.variables["defaultActorColor"] then
		self.defaultActorColor = self.story.variables["defaultActorColor"]
	else 
		self.defaultActorColor = "white"
	end
	self.actors = {
		smiley = {
			happy = "images/smiley_happy.jpg",
			angry = "images/smiley_angry.jpg",
			sad = "images/smiley_sad.jpg"
		}
	}
	self:getDocument():Show()
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
		if RTX.Input.WasPressed(self.exports.up) then
			self.currentChoice = self.currentChoice - 1
			if self.currentChoice < 1 then self.currentChoice = self.maxChoices end
			self:presentChoices(self.story:getChoices())
		end
		
		if RTX.Input.WasPressed(self.exports.down) then
			self.currentChoice = self.currentChoice + 1
			if self.currentChoice > self.maxChoices then self.currentChoice = 1 end
			self:presentChoices(self.story:getChoices())
		end
		
		if RTX.Input.WasPressed(self.exports.next) then
			local choices = self.story:getChoices()
			if choices[self.currentChoice].tags then
				for _, tag in ipairs(choices[self.currentChoice].tags) do
					local args = {}
					for str in string.gmatch(tag, "[^%s]+") do
							table.insert(args, str)
					end
					if args[1] == "onselect" then
						RTX.Call(args[2], args[3])
						print("event " .. args[2] .. " emitted")
					end
				end
			end
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
	local actor = ""
	local color = self.defaultColor
	local actorColor = self.defaultActorColor
	local className = ""
	for _, paragraph in ipairs(paragraphs) do
		text = text .. "<br />" .. paragraph.text
		if paragraph.tags then
			for _, tag in ipairs(paragraph.tags) do
				local args = {}
				for str in string.gmatch(tag, "[^%s]+") do
						table.insert(args, str)
				end
				if args[1] == "call" then
					RTX.Call(args[2], args[3])
				elseif args[1] == "actor" then
					actor = args[2]
					if self.actors[args[2]] then
						if self.actors[args[2]][args[3]] then
							self:getDocument():GetElementById("portraitContainer").inner_rml =  "<img id=\"portrait\" src = \"" .. self.actors[args[2]][args[3]] .. "\"/>"
						else 
						print("actor " .. args[2] .. "'s emotion : " .. args[3] .. " not found.")
						end
					else 
					print("actor " .. args[2] .. " not found.")
					end
				elseif args[1] == "color" then
					color = args[2]
				elseif args[1] == "actorColor" then
					actorColor = args[2]
				elseif args[1] == "class" then
					className = args[2]
				end
			end
		end
	end
	self.currentActor = actor
	self:getDocument():GetElementById("actor").style.color = actorColor
	self:getDocument():GetElementById("paragraphs").style.color = color
	self:getDocument():GetElementById("paragraphs").inner_rml ="<div class = \"" .. className .. "\">" .. text .. "</div>"
	self:getDocument():GetElementById("actor").inner_rml = actor
end

function DialoguePlayer:presentChoices(choices)
	local text = ""
	self.maxChoices = #choices
	for i, choice in ipairs(choices) do
		local selection = ""
		if self.currentChoice == i then
			selection = "> "
		end
		local choiceColor = ""
		local className = ""
		if choice.tags then
			for _, tag in ipairs(choice.tags) do
				local args = {}
				for str in string.gmatch(tag, "[^%s]+") do
						table.insert(args, str)
				end
				if args[1] == "color" then
					choiceColor = args[2]
				elseif args[1] == "class" then
					className = args[2]
				end
			end
		end
		text = text .. "<br />" ..  selection .. "<div class = \"" .. className 
		if choiceColor ~= "" then
			text = text .. "\" style = \"color:".. choiceColor  
		end
		text = text .. "\">" .. choice.text .. "</div>"
	end
	self:getDocument():GetElementById("choices").inner_rml = text
end

function DialoguePlayer:onEnd(entity)
end

function DialoguePlayer:destroy(entity)
end

return DialoguePlayer
