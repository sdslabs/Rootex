require("rootex/script/scripts/dialogue_node")

QuestionDialogue = class(DialogueNode)

QuestionDialogue.displayDocument = rmlui.contexts["default"]:LoadDocument("rootex/script/scripts/question_dialogue.rml")

function QuestionDialogue:init(question, answers, answerNodes)
	self.question = question
	self.answers = answers
	self.answerNodes = answerNodes
	self.currentChoice = 1
end

function QuestionDialogue:refreshDocument()
	QuestionDialogue.displayDocument:GetElementById("dialogue_question").inner_rml = self.question
	QuestionDialogue.displayDocument:GetElementById("dialogue_answer").inner_rml = self.answers[self.currentChoice]
end

function QuestionDialogue:getDocument()
	self:refreshDocument()
	return QuestionDialogue.displayDocument
end

function QuestionDialogue:handleInput(input)
	if input == DialogueInput.Next then
		return self.answerNodes[self.currentChoice]
	elseif input == DialogueInput.ChoiceUp then
		if self.currentChoice > 1 then self.currentChoice = self.currentChoice - 1 end
		self:refreshDocument()
	elseif input == DialogueInput.ChoiceDown then
		if self.currentChoice > 1 then self.currentChoice = self.currentChoice - 1 end
		self:refreshDocument()
	end
	return nil
end
