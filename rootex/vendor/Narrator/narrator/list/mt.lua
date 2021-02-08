--
-- Dependencies

local lume = require('narrator.libs.lume')

--
-- Metatable

local mt = { lists = { } }

function mt.__tostring(self)
  local pool = { }

  local listKeys = { }
  for key, _ in pairs(self) do
    table.insert(listKeys, key)
  end
  table.sort(listKeys)

  for i = 1, #listKeys do
    local listName = listKeys[i]
    local listItems = self[listName]
    for index = 1, #mt.lists[listName] do
      pool[index] = pool[index] or { }
      local itemName = mt.lists[listName][index]
      if listItems[itemName] == true then
        table.insert(pool[index], 1, itemName)
      end
    end
  end

  local items = { }

  for _, titles in ipairs(pool) do
    for _, title in ipairs(titles) do
      table.insert(items, title)
    end
  end

  return table.concat(items, ', ')
end

--
-- Operators

function mt.__add(lhs, rhs) -- +
  if type(rhs) == 'table' then
    return mt.__addList(lhs, rhs)
  elseif type(rhs) == 'number' then
    return mt.__shiftByNumber(lhs, rhs)
  else
    error('Attempt to sum the list with ' .. type(rhs))
  end
end

function mt.__sub(lhs, rhs) -- -
  if type(rhs) == 'table' then
    return mt.__subList(lhs, rhs)
  elseif type(rhs) == 'number' then
    return mt.__shiftByNumber(lhs, -rhs)
  else
    error('Attempt to sub the list with ' .. type(rhs))
  end
end

function mt.__mod(lhs, rhs) -- % (contain)
  if type(rhs) ~= 'table' then
    error('Attempt to check content of the list for ' .. type(rhs))
  end

  for listName, listItems in pairs(rhs) do
    if lhs[listName] == nil then return false end
    for itemName, itemValue in pairs(listItems) do
      if (lhs[listName][itemName] or false) ~= itemValue then return false end
    end  
  end

  return true
end

function mt.__pow(lhs, rhs) -- ^ (intersection)
  if type(rhs) ~= 'table' then
    error('Attempt to interselect the list with ' .. type(rhs))
  end

  local intersection = { }
  
  for listName, listItems in pairs(lhs) do
    for itemName, itemValue in pairs(listItems) do
      local left = lhs[listName][itemName]
      local right = (rhs[listName] or { })[itemName]
      if left == true and right == true then
        intersection[listName] = intersection[listName] or { }
        intersection[listName][itemName] = true
      end
    end
  end

  return intersection
end

function mt.__len(self) -- #
  local len = 0

  for listName, listItems in pairs(self) do
    for itemName, itemValue in pairs(listItems) do
      if itemValue == true then len = len + 1 end
    end
  end

  return len
end

function mt.__eq(lhs, rhs) -- ==
  if type(rhs) ~= 'table' then
    error('Attempt to compare the list with ' .. type(rhs))
  end

  local function keysCount(object) 
    local count = 0
    for _, _ in pairs(object) do
      count = count + 1
    end  
    return count
  end

  local leftListsCount = keysCount(lhs)
  local rightListsCount = keysCount(rhs)
  if leftListsCount ~= rightListsCount then
    return false
  end

  for listName, leftItems in pairs(lhs) do
    local rightItems = rhs[listName]
    if rightItems == nil then
      return false
    end

    local leftItemsCount = keysCount(leftItems)
    local rightItemsCount = keysCount(rightItems)
  
    if leftItemsCount ~= rightItemsCount then
      return false
    end
  end

  return mt.__mod(lhs, rhs)
end

function mt.__lt(lhs, rhs) -- <
  if type(rhs) ~= 'table' then
    error('Attempt to compare the list with ' .. type(rhs))
  end

  -- LEFT < RIGHT means "the smallest value in RIGHT is bigger than the largest values in LEFT"
  
  local minLeft = mt.minValueOf(lhs, true)
  local maxRight = mt.maxValueOf(rhs, true)

  return minLeft < maxRight
end

function mt.__le(lhs, rhs) -- <=
  if type(rhs) ~= 'table' then
    error('Attempt to compare the list with ' .. type(rhs))
  end

  -- LEFT => RIGHT means "the smallest value in RIGHT is at least the smallest value in LEFT,
  --                  and the largest value in RIGHT is at least the largest value in LEFT".

  local minRight = mt.minValueOf(rhs, true)
  local minLeft = mt.minValueOf(lhs, true)
  local maxRight = mt.maxValueOf(rhs, true)
  local maxLeft = mt.maxValueOf(lhs, true)

  return minRight >= minLeft and maxRight >= maxLeft
end

--
-- Custom operators

function mt.__addList(lhs, rhs)
  local result = lume.clone(lhs)

  for listName, listItems in pairs(rhs) do
    result[listName] = result[listName] or { }
    for itemName, itemValue in pairs(listItems) do
      result[listName][itemName] = itemValue
    end
  end

  return result
end

function mt.__subList(lhs, rhs)
  local result = lume.clone(lhs)

  for listName, listItems in pairs(rhs) do
    if lhs[listName] ~= nil then
      for itemName, _ in pairs(listItems) do
        lhs[listName][itemName] = nil
      end  
    end
  end

  return mt.removeEmptiesInList(result)
end

function mt.__shiftByNumber(list, number)
  local result = { }

  for listName, listItems in pairs(list) do
    result[listName] = { }
    for index, itemName in ipairs(mt.lists[listName]) do
      if listItems[itemName] == true then
        local nextItem = mt.lists[listName][index + number]
        if nextItem ~= nil then
          result[listName][nextItem] = true
        end
      end
    end
  end

  return mt.removeEmptiesInList(result)
end

--
-- Helpers

function mt.removeEmptiesInList(list)
  local result = lume.clone(list)

  for listName, listItems in pairs(list) do
    if next(listItems) == nil then
      result[listName] = nil
    end
  end

  return result
end

function mt.minValueOf(list, raw)
  local minIndex = 0
  local minValue = { }

  local listKeys = { }
  for key, _ in pairs(list) do
    table.insert(listKeys, key)
  end
  table.sort(listKeys)

  for i = 1, #listKeys do
    local listName = listKeys[i]
    local listItems = list[listName]
    for itemName, itemValue in pairs(listItems) do
      if itemValue == true then
        local index = lume.find(mt.lists[listName], itemName)
        if index and index < minIndex or minIndex == 0 then
          minIndex = index
          minValue = { [listName] = { [itemName] = true } }
        end
      end
    end
  end

  return raw and minIndex or minValue
end

function mt.maxValueOf(list, raw)
  local maxIndex = 0
  local maxValue = { }

  local listKeys = { }
  for key, _ in pairs(list) do
    table.insert(listKeys, key)
  end
  table.sort(listKeys)

  for i = 1, #listKeys do
    local listName = listKeys[i]
    local listItems = list[listName]
    for itemName, itemValue in pairs(listItems) do
      if itemValue == true then
        local index = lume.find(mt.lists[listName], itemName)
        if index and index > maxIndex or maxIndex == 0 then
          maxIndex = index
          maxValue = { [listName] = { [itemName] = true } }
        end
      end
    end
  end

  return raw and maxIndex or maxValue
end

function mt.randomValueOf(list)
  local items = { }

  local listKeys = { }
  for key, _ in pairs(list) do
    table.insert(listKeys, key)
  end
  table.sort(listKeys)

  for i = 1, #listKeys do
    local listName = listKeys[i]
    local listItems = list[listName]
    local itemsKeys = { }
    for key, _ in pairs(listItems) do
      table.insert(itemsKeys, key)
    end    
    table.sort(itemsKeys)

    for i = 1, #itemsKeys do
      local itemName = itemsKeys[i]
      local itemValue = listItems[itemName]
      if itemValue == true then
        local result = { [listName] = { [itemName] = true } }
        table.insert(items, result)
      end
    end
  end

  local randomIndex = math.random(1, #items)
  return items[randomIndex]
end

function mt.firstRawValueOf(list)
  local result = 0
  
  for listName, listItems in pairs(list) do
    for itemName, itemValue in pairs(listItems) do
      if itemValue == true then
        local index = lume.find(mt.lists[listName], itemName)
        if index then
          result = index
          break
        end
      end
    end
  end

  return result
end

function mt.posibleValuesOf(list)
  local result = { }

  for listName, listItems in pairs(list) do
    local subList = { }
    for _, itemName in ipairs(mt.lists[listName]) do
      subList[itemName] = true
    end
    result[listName] = subList
  end

  return result
end

function mt.rangeOf(list, min, max)
  if type(min) ~= 'table' and type(min) ~= 'number' then
    error('Attempt to get a range with incorrect min value of type ' .. type(min))
  end
  if type(max) ~= 'table' and type(max) ~= 'number' then
    error('Attempt to get a range with incorrect max value of type ' .. type(max))
  end

  local result = { }
  local allList = mt.posibleValuesOf(list)
  local minIndex = type(min) == 'number' and min or mt.firstRawValueOf(min)
  local maxIndex = type(max) == 'number' and max or mt.firstRawValueOf(max)

  for listName, listItems in pairs(allList) do
    for itemName, itemValue in pairs(listItems) do
      local index = lume.find(mt.lists[listName], itemName)
      if index and index >= minIndex and index <= maxIndex and list[listName][itemName] == true then
        result[listName] = result[listName] or { }
        result[listName][itemName] = true
      end
    end
  end

  return result
end

function mt.invert(list)
  local result = mt.posibleValuesOf(list)

  for listName, listItems in pairs(list) do
    for itemName, itemValue in pairs(listItems) do
      if itemValue == true then
        result[listName][itemName] = nil
      end
    end
  end

  return result
end

return mt