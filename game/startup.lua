class = require("game/assets/scripts/utility/middleclass")
require("game/assets/scripts/utility/common")
require("game/assets/scripts/utility/dialogue/dialogue_node")
require("game/assets/scripts/utility/dialogue/characters")
require("game/assets/scripts/utility/dialogue/one_off_dialogue")
require("game/assets/scripts/utility/dialogue/question_dialogue")

print(package.path)
print(lpeg)

local narrator = require("narrator.narrator")
local book = narrator.parseFile("game/assets/stories/game.ink")

-- Init a story from the book
local story = narrator.initStory(book)

-- Begin the story
story:begin()

while story:canContinue() do
  
  -- Get current paragraphs to output
  local paragraphs = story:continue()

  for _, paragraph in ipairs(paragraphs) do
    local text = paragraph.text

    -- You can handle tags as you like, but we attach them to text here.
    if paragraph.tags then
      text = text .. ' #' .. table.concat(paragraph.tags, ' #')
    end

    -- Output text to the player
    print(text)
  end

  -- If there is no choice it seems like the game is over
  if not story:canChoose() then break end
  
  -- Get available choices and output them to the player
  local choices = story:getChoices()
  for i, choice in ipairs(choices) do
    print(i .. ') ' .. choice.text)
  end

  -- Read the choice from the player input
  local answer = tonumber(io.read())

  -- Send answer to the story to generate new paragraphs
  story:choose(answer)
end
