local narrator, describe, it, assert = ...

local content = [[
  VAR x = 1
  * [Change x to 1]
  * [Change x to 2]
  * [Change x to 3]
  x = { x }
]]

local book = narrator.parseBook(content)
local story = narrator.initStory(book)

story:begin()

it('The x is equal to 1.', function()
  local x = story.variables['x']
  assert.equal(x, 1)
end)

it('The x is changed to 3.', function()
  local answer = 3
  story.variables['x'] = answer
  story:choose(answer)

  local paragraphs = story:continue()
  assert.equal(#paragraphs, 1)
  assert.equal(paragraphs[1].text, 'x = 3')
end)

