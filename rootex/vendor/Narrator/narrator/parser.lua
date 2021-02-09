--
-- Dependencies

local lume = require('narrator.libs.lume')
local enums = require('narrator.enums')

-- Safe lpeg requiring
local lpegName = 'lpeg'
if not pcall(require, lpegName) then return false end
local lpeg = require(lpegName)

--
-- LPeg

local S, C, P, V = lpeg.S, lpeg.C, lpeg.P, lpeg.V
local Cb, Ct, Cc, Cg = lpeg.Cb, lpeg.Ct, lpeg.Cc, lpeg.Cg
lpeg.locale(lpeg)

--
-- Parser

local Parser = { }
local Constructor = { }

--- Parse Ink content
-- @param content string: ink content to parse
-- @return table: a book
function Parser.parse(content)

  --
  -- Basic patterns

  local function getLength(array) return #array end

  local eof = -1
  local sp = S(' \t') ^ 0
  local ws = S(' \t\r\n') ^ 0
  local nl = S('\r\n') ^ 1
  local none = Cc(nil)

  local divertSign = P'->'
  local gatherMark = sp * C('-' - divertSign)
  local gatherLevel = Cg(Ct(gatherMark ^ 1) / getLength + none, 'level')
  
  local stickyMarks = Cg(Ct((sp * C('+')) ^ 1) / getLength, 'level') * Cg(Cc(true), 'sticky')
  local choiceMarks = Cg(Ct((sp * C('*')) ^ 1) / getLength, 'level') * Cg(Cc(false), 'sticky')
  local choiceLevel = stickyMarks + choiceMarks

  local id = (lpeg.alpha + '_') * (lpeg.alnum + '_') ^ 0
  local label = Cg('(' * sp * C(id) * sp * ')', 'label')
  local address = id * ('.' * id) ^ -2
  local divert = Cg(divertSign * sp * C(address), 'divert')
  local divertToNothing = divertSign * none
  local tag = '#' * sp * V'text'
  local tags = Cg(Ct(tag * (sp * tag) ^ 0), 'tags')

  local todo = sp * 'TODO:' * (1 - nl) ^ 0
  local commentLine = sp * '//' * sp * (1 - nl) ^ 0
  local commentMulti = sp * '/*' * ((P(1) - '*/') ^ 0) * '*/'
  local comment = commentLine + commentMulti

  local multilineEnd = ws * '}'

  --
  -- Dynamic patterns and evaluation helpers

  local function itemType(type)
    return Cg(Cc(type), 'type')
  end

  local function balancedMultilineItem(isRestricted)
    local isRestricted = isRestricted ~= nil and isRestricted or false
    local paragraph = isRestricted and V'restrictedParagraph' or V'paragraph'
    return sp * paragraph ^ -1 * sp * V'multilineItem' * sp * paragraph ^ -1 * ws
  end

  local function sentenceBefore(excluded, tailed)
    local tailed = tailed or false
    local character = P(1 - S(' \t')) - excluded
    local pattern = (sp * character ^ 1) ^ 1
    local withTail = C(pattern * sp)
    local withoutTail = C(pattern) * sp
    local withoutTailAlways = C(pattern) * sp * #(tags + nl)
    return withoutTailAlways + (tailed and withTail or withoutTail)
  end

  local function unwrapAssignment(assignment)
    local unwrapped = assignment
    unwrapped = unwrapped:gsub('([%w_]*)%s*([%+%-])[%+%-]', '%1 = %1 %2 1')
    unwrapped = unwrapped:gsub('([%w_]*)%s*([%+%-])=%s*(.*)', '%1 = %1 %2 %3')
    local name, value = unwrapped:match('([%w_]*)%s*=%s*(.*)')
    return name, value
  end

  --
  -- Grammar rules

  local inkGrammar = P({ 'root',

    -- Root

    root = V'items' + eof,
    items = Ct(V'item' ^ 0),

    item = balancedMultilineItem() + V'singlelineItem',
    singlelineItem = sp * (V'global' + V'statement' + V'paragraph') * ws,
    multilineItem = ('{' * sp * (V'sequence' + V'switch') * sp * multilineEnd) - V'inlineCondition',

    -- Global declarations

    global =
      Ct(V'inclusion' * itemType('inclusion')) +
      Ct(V'list' * itemType('list')) +
      Ct(V'constant' * itemType('constant')) +
      Ct(V'variable' * itemType('variable'))
    ,

    inclusion = 'INCLUDE ' * sp * Cg(sentenceBefore(nl + comment), 'filename'),
    list = 'LIST ' * sp * V'assignmentPair',
    constant = 'CONST ' * sp * V'assignmentPair',
    variable = 'VAR ' * sp * V'assignmentPair',

    -- Statements

    statement = 
      Ct(V'assignment' * itemType('assignment')) + 
      Ct(V'knot' * itemType('knot')) +
      Ct(V'stitch' * itemType('stitch')) +
      Ct(V'choice' * itemType('choice')) +
      comment + todo
    ,
    
    sectionName = C(id) * sp * P'=' ^ 0,
    knot = P'==' * (P'=' ^ 0) * sp * Cg(V'sectionName', 'knot'),
    stitch = '=' * sp * Cg(V'sectionName', 'stitch'),

    assignment = gatherLevel * sp * '~' * sp * V'assignmentTemp' * sp * V'assignmentPair',
    assignmentTemp = Cg('temp' * Cc(true) + Cc(false), 'temp'),
    assignmentPair = Cg(sentenceBefore(nl + comment) / unwrapAssignment, 'name') * Cg(Cb('name') / 2, 'value'),

    choiceCondition = Cg(V'expression' + none, 'condition'),
    choiceFallback = choiceLevel * sp * V'labelOptional' * sp * V'choiceCondition' * sp * (divert + divertToNothing) * sp * V'tagsOptional',
    choiceNormal = choiceLevel * sp * V'labelOptional' * sp * V'choiceCondition' * sp * Cg(V'text', 'text') * divert ^ -1 * sp * V'tagsOptional',
    choice = V'choiceFallback' + V'choiceNormal',

    -- Paragraph

    paragraph = Ct(gatherLevel * sp * (V'paragraphLabel' + V'paragraphText' + V'paragraphTags') * itemType('paragraph')),
    paragraphLabel = label * sp * Cg(V'textOptional', 'parts') * sp * V'tagsOptional',
    paragraphText = V'labelOptional' * sp * Cg(V'textComplex', 'parts') * sp * V'tagsOptional',
    paragraphTags = V'labelOptional' * sp * Cg(V'textOptional', 'parts') * sp * tags,
    
    labelOptional = label + none,
    textOptional = V'textComplex' + none,
    tagsOptional = tags + none,

    textComplex = Ct((Ct(
      Cg(V'inlineCondition', 'condition') + 
      Cg(V'inlineSequence', 'sequence') + 
      Cg(V'expression', 'expression') +
      Cg(V'text' + ' ', 'text') * (divert ^ -1) + divert
    ) - V'multilineItem') ^ 1),

    text = sentenceBefore(nl + divert + comment + tag + S'{|}', true) - V'statement',

    -- Inline expressions, conditions, sequences

    expression = '{' * sp * sentenceBefore('}' + nl) * sp * '}',

    inlineCondition = '{' * sp * Ct(V'inlineIfElse' + V'inlineIf') * sp * '}',
    inlineIf = Cg(sentenceBefore(S':}' + nl), 'condition') * sp * ':' * sp * Cg(V'textComplex', 'success'),
    inlineIfElse = (V'inlineIf') * sp * '|' * sp * Cg(V'textComplex', 'failure'),
    
    inlineAltEmpty = Ct(Ct(Cg(sp * Cc'', 'text') * sp * divert ^ -1)),
    inlineAlt = V'textComplex' + V'inlineAltEmpty',
    inlineAlts = Ct(((sp * V'inlineAlt' * sp * '|') ^ 1) * sp * V'inlineAlt'),
    inlineSequence = '{' * sp * (
    '!' * sp * Ct(Cg(V'inlineAlts', 'alts') * Cg(Cc('once'), 'sequence')) +
    '&' * sp * Ct(Cg(V'inlineAlts', 'alts') * Cg(Cc('cycle'), 'sequence')) +
    '~' * sp * Ct(Cg(V'inlineAlts', 'alts') * Cg(Cc('stopping'), 'sequence') * Cg(Cc(true),  'shuffle')) +
           Ct(Cg(V'inlineAlts', 'alts') * Cg(Cc('stopping'), 'sequence'))
    ) * sp * '}',

    -- Multiline conditions and switches

    switch = Ct((V'switchComparative' + V'switchConditional') * itemType('switch')),

    switchComparative = Cg(V'switchCondition', 'expression') * ws * Cg(Ct((sp * V'switchCase') ^ 1), 'cases'),
    switchConditional = Cg(Ct(V'switchCasesHeaded' + V'switchCasesOnly'), 'cases'),
    
    switchCasesHeaded = V'switchIf' * ((sp * V'switchCase') ^ 0),
    switchCasesOnly = ws * ((sp * V'switchCase') ^ 1),

    switchIf = Ct(Cg(V'switchCondition', 'condition') * ws * Cg(Ct(V'switchItems'), 'node')),
    switchCase = ('-' - divertSign) * sp * V'switchIf',
    switchCondition = sentenceBefore(':' + nl) * sp * ':' * sp * comment ^ -1,
    switchItems = (V'restrictedItem' - V'switchCase') ^ 1,

    -- Multiline sequences
    
    sequence = Ct((V'sequenceParams' * sp * nl * sp * V'sequenceAlts') * itemType('sequence')),

    sequenceParams = (
      V'sequenceShuffleOptional' * sp * V'sequenceType' +
      V'sequenceShuffle' * sp * V'sequenceType' +
      V'sequenceShuffle' * sp * V'sequenceTypeOptional'
    ) * sp * ':' * sp * comment ^ -1,

    sequenceShuffleOptional = V'sequenceShuffle' + Cg(Cc(false), 'shuffle'),
    sequenceShuffle = Cg(P'shuffle' / function() return true end, 'shuffle'),

    sequenceTypeOptional = V'sequenceType' + Cg(Cc'cycle', 'sequence'),
    sequenceType = Cg(P'cycle' + 'stopping' + 'once', 'sequence'),

    sequenceAlts = Cg(Ct((sp * V'sequenceAlt') ^ 1), 'alts'),
    sequenceAlt = ('-' - divertSign) * ws * Ct(V'sequenceItems'),
    sequenceItems = (V'restrictedItem' - V'sequenceAlt') ^ 1,

    -- Restricted items inside multiline items

    restrictedItem = balancedMultilineItem(true) + V'restrictedSinglelineItem',
    restrictedSinglelineItem = sp * (V'global' + V'restrictedStatement' + V'restrictedParagraph' - multilineEnd) * ws,

    restrictedStatement = Ct(
      V'choice' * itemType('choice') +
      V'assignment' * itemType('assignment')
    ) + comment + todo,
    
    restrictedParagraph = Ct((
      Cg(V'textComplex', 'parts') * sp * V'tagsOptional' +
      Cg(V'textOptional', 'parts') * sp * tags
    ) * itemType('paragraph'))

  })

  --
  -- Result

  local parsedItems = inkGrammar:match(content)
  local book = Constructor.constructBook(parsedItems)
  return book
  
end

--
-- A book construction

function Constructor.constructBook(items)
  
  local construction = {
    currentKnot = '_',
    currentStitch = '_',
  }

  construction.book = {
    inclusions = { },
    lists = { },
    constants = { },
    variables = { },
    tree = { _ = { _ = { } } }
  }

  construction.book.version = {
    engine = enums.engineVersion,
    tree = 1
  }
  
  construction.nodesChain = {
    construction.book.tree[construction.currentKnot][construction.currentStitch]
  }

  Constructor.addNode(construction, items)
  Constructor.clear(construction.book.tree)

  return construction.book
end

function Constructor:addNode(items, isRestricted)
  local isRestricted = isRestricted ~= nil and isRestricted or false

  for _, item in ipairs(items) do

    if isRestricted then
      -- Are not allowed inside multiline blocks by Ink rules:
      -- a) nesting levels
      -- b) choices without diverts 

      item.level = nil
      if item.type == 'choice' and item.divert == nil then
        item.type = nil
      end
    end

    if item.type == 'inclusion' then
      -- filename
      Constructor.addInclusion(self, item.filename)
    elseif item.type == 'list' then
      -- name, value
      Constructor.addList(self, item.name, item.value)
    elseif item.type == 'constant' then
      -- name, value
      Constructor.addConstant(self, item.name, item.value)
    elseif item.type == 'knot' then
      -- knot
      Constructor.addKnot(self, item.knot)
    elseif item.type == 'stitch' then
      -- stitch
      Constructor.addStitch(self, item.stitch)
    elseif item.type == 'switch' then
      -- expression, cases
      Constructor.addSwitch(self, item.expression, item.cases)
    elseif item.type == 'sequence' then
      -- sequence, shuffle, alts
      Constructor.addSequence(self, item.sequence, item.shuffle, item.alts)
    elseif item.type == 'variable' or item.type == 'assignment' then
      -- level, name, value, temp
      Constructor.addAssignment(self, item.level, item.name, item.value, item.temp)
    elseif item.type == 'paragraph' then
      -- level, label, parts, tags
      Constructor.addParagraph(self, item.level, item.label, item.parts, item.tags)
    elseif item.type == 'choice' then
      -- level, sticky, label, condition, text, divert, tags
      Constructor.addChoice(self, item.level, item.sticky, item.label, item.condition, item.text, item.divert, item.tags)
    end
  end
end

function Constructor:addInclusion(filename)
  table.insert(self.book.inclusions, filename)
end

function Constructor:addList(name, value)
  local items = lume.array(value:gmatch('[%w_%.]+'))
  self.book.lists[name] = items

  local switched = lume.array(value:gmatch('%b()'))
  switched = lume.map(switched, function(item) return item:sub(2, #item - 1) end)
  self.book.variables[name] = { [name] = { } }
  lume.each(switched, function(item) self.book.variables[name][name][item] = true end)
end

function Constructor:addConstant(constant, value)
  local value = lume.deserialize(value)
  self.book.constants[constant] = lume.deserialize(value)
end

function Constructor:addKnot(knot)
  self.currentKnot = knot
  self.currentStitch = '_'

  local node = { }
  self.book.tree[self.currentKnot] = { [self.currentStitch] = node }
  self.nodesChain = { node }
end

function Constructor:addStitch(stitch)
  
  -- If a root stitch is empty we need to add a divert to the first stitch in the ink file.
  if self.currentStitch == '_' then
    local rootStitchNode = self.book.tree[self.currentKnot]._
    if #rootStitchNode == 0 then
      local divertItem = { divert = stitch }
      table.insert(rootStitchNode, divertItem)  
    end
  end

  self.currentStitch = stitch

  local node = { }
  self.book.tree[self.currentKnot][self.currentStitch] = node
  self.nodesChain = { node }
end

function Constructor:addSwitch(expression, cases)
  if expression then
    -- Convert switch cases to comparing conditions with expression
    for _, case in ipairs(cases) do
      if case.condition ~= 'else' then
        case.condition = expression .. '==' .. case.condition
      end
    end
  end

  local item = {
    condition = { },
    success = { }
  }

  for _, case in ipairs(cases) do
    if case.condition == 'else' then
      local failureNode = { }
      table.insert(self.nodesChain, failureNode)
      Constructor.addNode(self, case.node, true)
      table.remove(self.nodesChain)
      item.failure = failureNode
    else
      local successNode = { }
      table.insert(self.nodesChain, successNode)
      Constructor.addNode(self, case.node, true)
      table.remove(self.nodesChain)
      table.insert(item.success, successNode)
      table.insert(item.condition, case.condition)
    end
  end

  Constructor.addItem(self, nil, item)
end

function Constructor:addSequence(sequence, shuffle, alts)
  local item = {
    sequence = sequence,
    shuffle = shuffle and true or nil,
    alts = { }
  }

  for _, alt in ipairs(alts) do
    local altNode = { }
    table.insert(self.nodesChain, altNode)
    Constructor.addNode(self, alt, true)
    table.remove(self.nodesChain)
    table.insert(item.alts, altNode)
  end

  Constructor.addItem(self, nil, item)
end

function Constructor:addAssignment(level, name, value, temp)
  local item = {
    temp = temp or nil,
    var = name,
    value = value
  }

  Constructor.addItem(self, level, item)
end

function Constructor:addParagraph(level, label, parts, tags)
  local items = Constructor.convertParagraphPartsToItems(parts, true)
  items = items or { }
  
  -- If the paragraph has a label or tags we need to place them as the first text item.
  if label ~= nil or tags ~= nil then
    local firstItem

    if #items > 0 and items[1].condition == nil then
      firstItem = items[1]
    else
      firstItem = {  }
      table.insert(items, firstItem)
    end

    firstItem.label = label
    firstItem.tags = tags
  end

  for _, item in ipairs(items) do
    Constructor.addItem(self, level, item)
  end
end

function Constructor.convertParagraphPartsToItems(parts, isRoot)
  if parts == nil then return nil end

  local isRoot = isRoot ~= nil and isRoot or false
  local items = { }
  local item
  
  for index, part in ipairs(parts) do

    if part.condition then -- Inline condition part

      item = {
        condition = part.condition.condition,
        success = Constructor.convertParagraphPartsToItems(part.condition.success),
        failure = Constructor.convertParagraphPartsToItems(part.condition.failure)
      }

      table.insert(items, item)
      item = nil

    elseif part.sequence then -- Inline sequence part
      
      item = {
        sequence = part.sequence.sequence,
        shuffle = part.sequence.shuffle and true or nil,
        alts = { }
      }
      
      for _, alt in ipairs(part.sequence.alts) do
        table.insert(item.alts, Constructor.convertParagraphPartsToItems(alt))
      end

      table.insert(items, item)
      item = nil

    else -- Text, expression and divert may be

      local isDivertOnly = part.divert ~= nil and part.text == nil

      if item == nil then
        item = { text = (isRoot or isDivertOnly) and '' or '<>' }
      end

      if part.text then
        item.text = item.text .. part.text:gsub('%s+', ' ')
      elseif part.expression then
        item.text = item.text .. '#' .. part.expression .. '#'
      end

      if part.divert then
        item.divert = part.divert
        item.text = #item.text > 0 and (item.text .. '<>') or nil
        table.insert(items, item)
        item = nil
      else
        local next = parts[index + 1]
        local nextIsBlock = next and not (next.text or next.expression)

        if not next or nextIsBlock then
          if not isRoot or nextIsBlock then
            item.text = item.text .. '<>'
          end
          table.insert(items, item)
          item = nil  
        end
      end

    end
  end

  if isRoot then
    -- Add a safe prefix and suffix for correct conditions gluing
    
    local firstItem = items[1]
    if firstItem.text == nil and firstItem.divert == nil then
      table.insert(items, 1, { text = '' } )
    end
    
    local lastItem = items[#items]
    if lastItem.text == nil and lastItem.divert == nil then
      table.insert(items, { text = '' } )
    elseif lastItem.text ~= nil and lastItem.divert == nil then
      lastItem.text = lastItem.text:gsub('(.-)%s*$', '%1')
    end
  end

  return items
end

function Constructor:addChoice(level, sticky, label, condition, sentence, divert, tags)
  local item = {
    sticky = sticky or nil,
    condition = condition,
    label = label,
    divert = divert,
    tags = tags
  }

  if sentence == nil then
    item.choice = 0
  else
    local prefix, divider, suffix = sentence:match('(.*)%[(.*)%](.*)')
    prefix = prefix or sentence
    divider = divider or ''
    suffix = suffix or ''

    local text = (prefix .. suffix):gsub('%s+', ' ')
    local choice = (prefix .. divider):gsub('%s+', ' '):gsub('^%s*(.-)%s*$', '%1')

    if divert and #text > 0 and text:match('%S+') then
      text = text .. '<>'
    else
      text = text:gsub('^%s*(.-)%s*$', '%1')
    end
    
    item.text = text
    item.choice = choice
  end

  Constructor.addItem(self, level, item)

  if divert == nil then
    item.node = { }
    table.insert(self.nodesChain, item.node)
  end
end

function Constructor:addItem(level, item)
  local level = (level ~= nil and level > 0) and level or #self.nodesChain
  while #self.nodesChain > level do
    table.remove(self.nodesChain)
  end
  
  local node = self.nodesChain[#self.nodesChain]
  table.insert(node, item)
end

function Constructor.clear(tree)
  for knot, node in pairs(tree) do
    for stitch, node in pairs(node) do
      Constructor.clearNode(node)
    end
  end
end

function Constructor.clearNode(node)
  for index, item in ipairs(node) do
    
    -- Simplify text only items
    if item.text ~= nil and lume.count(item) == 1 then
      node[index] = item.text
    end
    
    if item.node ~= nil then
      -- Clear choice nodes
      if #item.node == 0 then
        item.node = nil
      else
        Constructor.clearNode(item.node)
      end
      
    end

    if item.success ~= nil then
      -- Simplify single condition
      if type(item.condition) == 'table' and #item.condition == 1 then
        item.condition = item.condition[1]
      end

      -- Clear success nodes
      if item.success[1] ~= nil and item.success[1][1] ~= nil then
        for index, successNode in ipairs(item.success) do
          Constructor.clearNode(successNode)
          if #successNode == 1 and type(successNode[1]) == 'string' then
            item.success[index] = successNode[1]
          end
        end

        if #item.success == 1 then
          item.success = item.success[1]
        end
      else
        Constructor.clearNode(item.success)
        if #item.success == 1 and type(item.success[1]) == 'string' then
          item.success = item.success[1]
        end   
      end

      -- Clear failure nodes
      if item.failure ~= nil then
        Constructor.clearNode(item.failure)
        if #item.failure == 1 and type(item.failure[1]) == 'string' then
          item.failure = item.failure[1]
        end     
      end
    end

    if item.alts ~= nil then
      for index, altNode in ipairs(item.alts) do
        Constructor.clearNode(altNode)
        if #altNode == 1 and type(altNode[1]) == 'string' then
          item.alts[index] = altNode[1]
        end
      end
    end
  end
end

return Parser