require("game/assets/scripts/utility/dialogue/dialogue_node")

QuestionDialogue = class("QuestionDialogue", DialogueNode)

QuestionDialogue.static.document = rmlui.contexts["default"]:LoadDocument("game/assets/ui/dialogue/question_dialogue.rml")
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
	local choicesRML = ""
	for i, choice in ipairs(self.answers) do
		local choiceFormat = "<div>{choice}</div>"
		if i == self.currentChoice then
			choiceFormat = "<div id='current_choice'>{choice}</div>"
			choice = "> " .. choice
		end
		choicesRML = choicesRML .. choiceFormat:gsub("{choice}", choice) .. "<br/>"
	end
	QuestionDialogue.static.document:GetElementById("dialogue_portrait"):SetAttribute("src", "../../..://dummy/" .. self.portrait.image)
	QuestionDialogue.static.document:GetElementById("dialogue_question").inner_rml = self.question
	QuestionDialogue.static.document:GetElementById("dialogue_answer").inner_rml = choicesRML
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
