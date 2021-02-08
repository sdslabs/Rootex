SEED = 2 { SEED_RANDOM(2) }
RANDOM(1, 100) = { RANDOM(1, 100) }
POW(2,4) = { POW(2, 4) }
INT(1.45) = { INT(1.45) }
FLOOR(2.4) = { FLOOR(2.4) }
FLOAT(3) = { FLOAT(3) }

* { CHOICE_COUNT() == 0 } [Choice 1]

{ "Yes, please." == "Yes, please." : True! | False! } 
{ "Yes, please." != "Yes, please." : True! | False! } 
{ "Yes, please" ? "ease" : True! | False! } 
{ "Yes, please" !? "ease" : True! | False! } 