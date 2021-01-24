--
-- Test cases

local runtime = {
  'continue',
  'observing',
  'binding',
  'set-get',
  'visits',
  'tags',
  'jumping',
  'save-load'
}

local units = {
  'inclusions',
  'comments',
  'knots',
  'stitches',

  'text-line',
  'text-lines',
  'text-tags',
  'text-glue',
    
  'choices-basic',
  'choices-tags',  
  'choices-conditional',
  'choices-sticky',
  'choices-fallback',  

  'branching',
  'nesting',
  'labels',
  'loop',
  'vars',
  'constants',
  'expressions',
  'queries',

  'conditions-inline',
  'alts-inline',
  'conditions-switch',
  'alts-blocks',
  
  'lists-basic',
  'lists-operators',
  'lists-queries',
}

local stories = {
  -- No complex stories at the moment
}

local cases = {
  runtime = runtime,
  units = units,
  stories = stories
}

local folderSeparator = package.config:sub(1, 1)
for folderName, folderCases in pairs(cases) do
  local itemsWithFoldes = { }
  for _, case in ipairs(folderCases) do
    table.insert(itemsWithFoldes, folderName .. folderSeparator .. case)
  end
  cases[folderName] = itemsWithFoldes
end

return cases