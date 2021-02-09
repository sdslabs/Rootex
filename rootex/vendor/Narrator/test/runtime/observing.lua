local narrator, describe, it, assert = ...

local content = [[
  VAR mood = "sadly"
  ~ mood = "sunny"
]]

local book = narrator.parseBook(content)
local story = narrator.initStory(book)

local mood

story:observe('mood', function(value)
  mood = value
end)

story:begin()

it('Sunny mood.', function()
  assert.equal(mood, 'sunny')
end)