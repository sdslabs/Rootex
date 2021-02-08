--
-- Testing with Busted library

--
-- Dependencies

require('busted.runner')()

if os.getenv('LOCAL_LUA_DEBUGGER_VSCODE') == '1' then
  require("lldebugger").start()
end

local bot = require('bot')
local narrator = require('narrator.narrator')
local lume = require('narrator.libs.lume')

--
-- Constants

local folderSeparator = package.config:sub(1, 1)
local testsFolder = 'test' .. folderSeparator

--- Make path for a .lua file
-- @param case string: an runtime test case
-- @return string: a .lua path
local function luaPath(case)
  return testsFolder .. case .. '.lua'
end

--- Make path for an .ink file
-- @param case string: an Ink test case
-- @return string: an .ink path
local function inkPath(case)
  return testsFolder .. case .. '.ink'
end

--- Make path for a .txt file
-- @param case string: an Ink test case
-- @param answers table: a sequence of answers (numbers)
-- @return string: a .txt path
local function txtPath(case, answers)
  local path = testsFolder .. case
  if answers and #answers > 0 then
    path = path .. folderSeparator .. table.concat(answers, "-")
  end
  path = path .. '.txt'
  return path
end

--- Get all possible sequences and logs of the case
-- @param case string: an Ink test case
-- @return table: an array of possible games { sequence, log }
local function getPossibleResults(case)
  local path = inkPath(case)
  local book = narrator.parseFile(path)
  
  local results = { }
  local sequences = { { } }
  local seqIndex

  local function instructor(choices, step)
    local curSeq = sequences[seqIndex]
    local answer = curSeq[step]
    
    if not answer then
      -- Transform a current sequence to branches for each available choice
      table.remove(sequences, seqIndex)

      for index, _ in ipairs(choices) do
        local newSeq = lume.concat(curSeq, { index })
        table.insert(sequences, newSeq)
      end

      -- Set a stop signal for the bot
      answer = -1
    end

    return answer
  end

  while #sequences > 0 do
    -- Iterate sequences and play them
    for index = 1, #sequences do
      local sequence = sequences[index]
      seqIndex = index

      -- Play the sequence
      local story = narrator.initStory(book)
      local log = bot.play(story, instructor)

      -- If the sequence was finished then save the result and mark it as finished
      if log then
        local result = { sequence = sequence, log = log }
        table.insert(results, result)
        sequences[index] = { isFinished = true }
      end
    end

    -- Remove finished sequences
    for index = #sequences, 1, -1 do
      local sequence = sequences[index]
      if sequence.isFinished then
        table.remove(sequences, index)
      end
    end    
  end

  return results
end

--- Create possible results for an Ink test case and save them to txt files
-- @param case string: an Ink test case
-- @param override bool: override a txt file if it already exists.
local function createTxtForInkCase(case, override)
  local override = override ~= nil and override or false
  local results = getPossibleResults(case)
  
  for _, result in ipairs(results) do
    local txtPath = txtPath(case, #results > 1 and result.sequence or nil)
    local file = io.open(txtPath, 'r')
    local isFileExists = file ~= nil
    if isFileExists then io.close(file) end

    if not isFileExists or override then
      local folderPath = txtPath:match('(.*' .. folderSeparator .. ')')
      local folder = io.open(folderPath, 'r')
      local isFolderExists = folder ~= nil
      if isFolderExists then
        io.close(folder)
      else
        os.execute('mkdir ' .. folderPath)
      end

      file = io.open(txtPath, 'w')
      assert(file, 'Has no access to the file at path  \'' .. txtPath .. '\'.')
      
      file:write(result.log)
      file:close()
    end
  end
end

--- Create possible results for Ink test cases and save them to txt files
-- @param cases table: an array of Ink test cases
-- @param override bool: override the txt file if it already exists
local function createTxtForInkCases(cases, override)
  for _, case in ipairs(cases) do
    createTxtForInkCase(case, override)
  end
end

--- Test an Ink case
-- @param case string: an Ink test case
local function testInkCase(case)
  describe('Test an Ink case \'' .. case .. '\'.', function()
    local path = inkPath(case)
    local book = narrator.parseFile(path)

    local results = getPossibleResults(case)
    for _, result in ipairs(results) do
      describe('Sequence is [' .. table.concat(result.sequence, "-") .. '].', function()
        local txtPath = txtPath(case, #results > 1 and result.sequence or nil)
        local file = io.open(txtPath, 'r')

        it('Checking results.', function()
          assert.is_not_nil(file)

          local expected = file:read('*all')
          file:close()
  
          assert.are.same(expected, result.log)
        end)
      end)
    end
  end)
end

--- Test Ink cases
-- @param cases table: an array of test cases
local function testInkCases(cases)
  for _, case in ipairs(cases) do
    testInkCase(case)
  end
end

--- Test a runtime case
-- @param case string: a runtime test case
local function testLuaCase(case)
  describe('Test a runtime case \'' .. case .. '\'.', function()
    local luaPath = luaPath(case)
    loadfile(luaPath)(narrator, describe, it, assert)
  end)
end

--- Test runtime cases
-- @param cases table: an array of runtime test cases
local function testLuaCases(cases)
  for _, case in ipairs(cases) do
    testLuaCase(case)
  end
end

local cases = require('test.cases')

local runtime = cases.runtime
local units = cases.units
local stories = cases.stories

-- createTxtForInkCases(units, true)
testLuaCases(runtime)
testInkCases(units)
testInkCases(stories)