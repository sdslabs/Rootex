local narrator, describe, it, assert = ...

local content = [[
  A root line
  === knot
  A knot line
  = stitch
  A stitch line
  === somewhere
  - (label) A label line
]]

local book = narrator.parseBook(content)
local story = narrator.initStory(book)

story:begin()

local paragraphs = story:continue()

it('Jump to label.', function()
  story:jumpTo('somewhere.label')
  local paragraphs = story:continue()
  assert.equal(#paragraphs, 1)
  assert.equal(paragraphs[1].text, 'A label line')
end)

it('Jump to stitch.', function()
  story:jumpTo('knot.stitch')
  local paragraphs = story:continue()
  assert.equal(#paragraphs, 1)
  assert.equal(paragraphs[1].text, 'A stitch line')
end)

it('Jump to knot.', function()
  story:jumpTo('knot')
  local paragraphs = story:continue()
  assert.equal(#paragraphs, 1)
  assert.equal(paragraphs[1].text, 'A knot line')
end)