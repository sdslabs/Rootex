require("game/assets/scripts/utility/dialogue/characters")

return DialogueBuilder()
:addTemplate("dialogue_template")
:add(OneOffDialogue(PersonA.neutral, "Start"))
:add(OneOffDialogue(PersonA.neutral, "> You arrive at Yasoinaba."))
:add(OneOffDialogue(PersonA.neutral, "Hey! Over here!"))
:add(OneOffDialogue(PersonA.happy, "Well, you’re more handsome in person than in your photo."))
:add(OneOffDialogue(PersonA.happy, "Welcome to Inaba."))
:add(OneOffDialogue(PersonA.neutral, "I’m Ryotaro Dojima. I’ll be looking after you."))
:add(QuestionDialogue(PersonA.neutral, "Let’s see… I’m your mother’s younger brother… and that about sums it up", {
    "It’s nice to meet you.",
    "It’s been a long time.",
    "……"
}, { 
    OneOffDialogue(PersonA.neutral, "We are happy to have you."),
    OneOffDialogue(PersonA.neutral, "Heh, very long indeed."),
    OneOffDialogue(PersonA.neutral, "Now then,") }))
:add(OneOffDialogue(PersonA.happy, "You probably don’t remember, but we’ve met."))
:add(OneOffDialogue(PersonA.neutral, "I’ve changed your diapers before, you know."))
:add(OneOffDialogue(PersonA.neutral, "Come one, let's head home."))
:finish()
