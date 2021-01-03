require("rootex/script/scripts/dialogue_node")

QuestionDialogue = class("QuestionDialogue", DialogueNode)

QuestionDialogue.static.document = rmlui.contexts["default"]:LoadDocument("rootex/script/scripts/question_dialogue.rml")
QuestionDialogue.static.document:Hide()

function QuestionDialogue:initialize(portrait, question, answers, answerNodes, exitLogic)
	DialogueNode.initialize(self, exitLogic)
	self.portrait = portrait
	self.question = question
	self.answers = answers
	self.answerNodes = answerNodes
	self.currentChoice = 1
end

function QuestionDialogue:refreshDocument()
	QuestionDialogue.static.document:GetElementById("dialogue_question").inner_rml = self.question
	
	local choicesRML = ""
	local choiceFormat = "<div class='choice'>{choice}</div>"
	for i, choice in ipairs(self.answers) do
		if i == self.currentChoice then
			choice = "> " .. choice
		end
		choicesRML = choicesRML .. choiceFormat:gsub("{choice}", choice) .. "<br/>"
	end
	QuestionDialogue.static.document:GetElementById("dialogue_answer").inner_rml = choicesRML
	
	local portraitFormat = [[<img src="../../..://dummy/{src}" width={width}em height={height}em />]]
	portraitFormat = portraitFormat
	:gsub("{src}", self.portrait.image)
	:gsub("{width}", self.portrait.width)
	:gsub("{height}", self.portrait.height)
	QuestionDialogue.static.document:GetElementById("dialogue_portrait").inner_rml = portraitFormat
end

function QuestionDialogue:getDocument()
	self:refreshDocument()
	return QuestionDialogue.static.document
end

function QuestionDialogue:handleInput(input)
	if input == DialogueInput.Next then
		DialogueNode.static.stack:push(self:getNextNode())
		return self.answerNodes[self.currentChoice]
	elseif input == DialogueInput.ChoiceDown then
		if self.currentChoice > 1 then self.currentChoice = self.currentChoice - 1 else self.currentChoice = #self.answers end
		self:refreshDocument()
	elseif input == DialogueInput.ChoiceUp then
		if self.currentChoice < #self.answers then self.currentChoice = self.currentChoice + 1 else self.currentChoice = 1 end
		self:refreshDocument()
	end
	return self
end
