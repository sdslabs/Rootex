local narrator, describe, it, assert = ...

local content = [[
  Line 1
  Line 2
  Line 3
  Line 4
  Line 5
  Line 6
]]

local book = narrator.parseBook(content)
local story = narrator.initStory(book)

story:begin()

it('Get one paragraph.', function()
  local paragraph = story:continue(1)
  assert.equal(paragraph.text, 'Line 1')
end)

it('Get two paragraphs.', function()
  local paragraphs = story:continue(2)
  assert.equal(#paragraphs, 2)
end)

it('Get remain paragraphs.', function()
  local paragraphs = story:continue()
  assert.equal(#paragraphs, 3)
end)