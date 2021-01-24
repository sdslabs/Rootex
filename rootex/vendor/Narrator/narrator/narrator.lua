--
-- Dependencies

local lume = require('narrator.libs.lume')
local enums = require('narrator.enums')
local parser = require('narrator.parser')
local Story = require('narrator.story')

--
-- Private

local folderSeparator = package.config:sub(1, 1)

--- Clears path from '.lua' and '.ink' extensions and replace '.' to '/' or '\'
-- @param path string: path to clear
-- @return string: a clean path
local function clearPath(path)
  local path = path:gsub('.lua$', '')
  local path = path:gsub('.ink$', '')

  if path:match('%.') and not path:match(folderSeparator) then
    path = path:gsub('%.', folderSeparator)
  end

  return path
end

--- Parse an Ink file to a string content
-- @param path string: path to an Ink file
-- @return string: a content string
local function readFile(path)
  local path = clearPath(path) .. '.ink'

  local file = io.open(path, 'r')
  assert(file, 'File doesn\'t exist: ' .. path)

  local content = file:read('*all')
  file:close()

  return content
end

--- Save a book to lua module
-- @param book table: a book
-- @param path string: a path to save
-- @return bool: success
local function saveBook(book, path)
  local path = clearPath(path)  .. '.lua'

  local data = lume.serialize(book)
  data = data:gsub('%[%d+%]=', '')
  data = data:gsub('[\"[%w_]+\"]', function(match) return
    match:sub(3, #match - 2)
  end)
  
  local file = io.open(path, 'w')
  if file == nil then
    return false
  end
  
  file:write('return ' .. data)
  file:close()
  
  return true
end

--- Merge a chapter to a book
-- @param book table: a parent book
-- @param chapter table: a chapter to merge
-- @return table: a book
local function merge(book, chapter)
  -- Check a engine version compatibility
  if chapter.version.engine and chapter.version.engine ~= enums.engineVersion then
    assert('Version ' .. chapter.version.engine .. ' of book isn\'t equal to the version ' .. enums.engineVersion .. ' of Narrator.')
  end

  -- Merge the root knot and it's stitch
  book.tree._._ = lume.concat(chapter.tree._._, book.tree._._)
  chapter.tree._._ = nil
  book.tree._ = lume.merge(chapter.tree._, book.tree._)
  chapter.tree._ = nil

  -- Merge a chapter to a book
  book.tree = lume.merge(book.tree or { }, chapter.tree or { })
  book.constants = lume.merge(book.constants or { }, chapter.constants or { })
  book.lists = lume.merge(book.lists or { }, chapter.lists or { })
  book.variables = lume.merge(book.variables or { }, chapter.variables or { })
  
  return book
end

--
-- Public

local Narrator = { }

--- Parse a book from an Ink file
-- Use parsing in development, but prefer already parsed and stored books in production
-- Required: lpeg, io
-- @param path string: path to an Ink file
-- @param params table: parameters { save }
-- @param params.save bool: save a parsed book to a lua file
-- @return a book
function Narrator.parseFile(path, params)
  local params = params or { save = false }
  assert(parser, "Can't parse anything without lpeg, sorry.")

  local content = readFile(path)
  local book = parser.parse(content)
  
  for _, inclusion in ipairs(book.inclusions) do
    local folderPath = path:match('(.*' .. folderSeparator .. ')')
    local inclusionPath = folderPath .. clearPath(inclusion) .. '.ink'
    local chapter = Narrator.parseFile(inclusionPath)
    merge(book, chapter)
  end  

  if params.save then
    saveBook(book, path)
  end

  return book
end

--- Parse a book from Ink content
-- Use parsing in development, but prefer already parsed and stored books in production
-- Required: lpeg
-- @param content string: root Ink content
-- @param inclusions table: an array of strings with Ink content inclusions
-- @return table: a book
function Narrator.parseBook(content, inclusions)
  local inclusions = inclusions or { }
  assert(parser, "Can't parse anything without a parser.")
  
  local book = parser.parse(content)
  
  for _, inclusion in ipairs(inclusions) do
    local chapter = parser.parse(inclusion)
    merge(book, chapter)
  end  

  return book
end

--- Init a story from a book
-- @param book table: a book
-- @return table: a story
function Narrator.initStory(book)
  local story = Story(book)
  return story
end

return Narrator