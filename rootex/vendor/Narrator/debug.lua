--
-- Script for debugging in VSCode with
-- Local Lua Debugger by Tom Blind (https://github.com/tomblind/local-lua-debugger-vscode)

-- Dependencies
local narrator = require('narrator.narrator')
local bot = require('bot')

-- Loading
-- local book = require('stories.debug')
-- local book = narrator.parseBook('Hello world!', { '=== one === \n text 1', '=== two === \n text 2' })
local book = narrator.parseFile('stories.debug', { save = false })

local story = narrator.initStory(book)
local answers = { }

-- Choice instructor for a bot
local function instructor(choices, step)
  local answer = answers[step]
  if answer == nil then
    math.randomseed(os.clock() * 1000)
    answer = math.random(1, #choices)
  end
  return answer
end

-- Game
print('--- Game started ---\n')
bot.play(story, instructor, { print = true })
print('\n--- Game over ---')