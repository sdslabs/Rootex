--
-- Enums

local enums = {

  -- Release version of Narrator
  engineVersion = 1,

  -- Story item type
  item = { 
    text = 1,
    alts = 2,
    choice = 3,
    condition = 4,
    variable = 5
  },
  
  -- Alternatives sequence type
  sequence = {
    cycle = 1,
    stopping = 2,
    once = 3
  },

  -- Runtime reading mode
  readMode = { 
    text = 1,
    choices = 2,
    gathers = 3,
    quit = 4
  }

}

return enums