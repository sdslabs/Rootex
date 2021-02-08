local narrator, describe, it, assert = ...

local content = [[
  VAR x = 1
  * (choice) [Hello]
  - (hello) Hello world!
  -> knot
  === knot
  = stitch
  ~ x = 2
  ~ temp y = 3
  A line 1
  A line 2
  A line 3
  * Just a road to hell -> END
  * The best road to hell -> END
]]

local book = narrator.parseBook(content)

local savedState

it('Saving', function()
  local story = narrator.initStory(book)
  story:begin()
  story:continue()
  story:choose(1)
  story:continue(2)

  savedState = story:saveState()

  local expectedPath = { knot = 'knot', stitch = 'stitch' }
  assert.are.same(savedState.path, expectedPath)
  assert.equal(savedState.variables['x'], 2)
  assert.equal(savedState.temp['y'], 3)
  assert.equal(savedState.visits._._.hello, 1)
  assert.equal(#savedState.output, 2)
  assert.equal(#savedState.paragraphs, 2)
  assert.equal(#savedState.choices, 2)
end)

it('Loading.', function()
  local story = narrator.initStory(book)
  story:begin()
  story:loadState(savedState)
  
  local expectedPath = { knot = 'knot', stitch = 'stitch' }
  assert.are.same(story.currentPath, expectedPath)
  assert.equal(story.variables['x'], 2)
  assert.equal(story.temp['y'], 3)
  assert.equal(story:getVisits('hello'), 1)
  assert.equal(#story.output, 2)

  local paragraphs = story:continue()
  local choices = story:getChoices()

  assert.equal(#paragraphs, 2)
  assert.equal(#choices, 2)
end)