require("rootex/script/scripts/dialogue_node")

QuestionDialogue = class("QuestionDialogue", DialogueNode)

function QuestionDialogue:initialize(question, answers, answerNodes, exitLogic)
	self.question = question
	self.answers = answers
	self.answerNodes = answerNodes
	self.currentChoice = 1
	self.document = rmlui.contexts["default"]:LoadDocument("rootex/script/scripts/question_dialogue.rml")
	self.document:Hide()
	self.exitLogic = exitLogic
end

function QuestionDialogue:refreshDocument()
	self.document:GetElementById("dialogue_question").inner_rml = self.question
	
	local choicesRML = ""
	local choiceFormat = "<div class='choice'>{choice}</div>"
	for i, choice in ipairs(self.answers) do
		if i == self.currentChoice then
			choice = "> " .. choice
		end
		choicesRML = choicesRML .. choiceFormat:gsub("{choice}", choice) .. "<br/>"
	end
	self.document:GetElementById("dialogue_answer").inner_rml = choicesRML		
end

function QuestionDialogue:getDocument()
	self:refreshDocument()
	return self.document
end

function QuestionDialogue:handleInput(input)
	if input == DialogueInput.Next then
		if self.exitLogic ~= nil then self.exitLogic(self) end
		return self.answerNodes[self.currentChoice]
	elseif input == DialogueInput.ChoiceDown then
		if self.currentChoice > 1 then self.currentChoice = self.currentChoice - 1 else self.currentChoice = #self.answers end
		self:refreshDocument()
	elseif input == DialogueInput.ChoiceUp then
		if self.currentChoice < #self.answers then self.currentChoice = self.currentChoice + 1 else self.currentChoice = 1 end
		self:refreshDocument()
	end
	return nil
end
