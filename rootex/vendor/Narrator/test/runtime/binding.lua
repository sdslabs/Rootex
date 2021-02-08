local narrator, describe, it, assert = ...

local content = [[
  { beep() }
  { sum(1, 2) }
  { didSolvePuzzle("labirint") }
]]

local book = narrator.parseBook(content)
local story = narrator.initStory(book)

local isBeeped = false
local puzzles = { }

story:bind('beep', function()
  isBeeped = true
end)

story:bind('sum', function(x, y)
  return x + y
end)

story:bind('didSolvePuzzle', function(puzzle)
  puzzles[puzzle] = true
end)

story:begin()

it('Was a beep?', function()
  assert.is_true(isBeeped)
end)

it('Sum is equal to 3.', function()
  local paragraphs = story:continue()
  assert.equal(#paragraphs, 1)
  assert.equal('3', paragraphs[1].text)
end)

it('Labirint is sovled.', function()
  local puzzleIsSolved = puzzles['labirint']
  assert.is_true(puzzleIsSolved)
end)