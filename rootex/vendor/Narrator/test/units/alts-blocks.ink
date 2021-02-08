VAR counter = 1
-> casino

= casino
=== { counter }
At the table, I drew a card. <>
{ stopping:
    -  
    I entered the casino.
	-  Okay. <>
    I entered the casino again.
	-  { counter }. <>
    
    Once more, I went inside.
}
~ counter++
{ counter < 5 : -> casino | -> cycle }

= cycle
=== { counter }
{ cycle:
	- I held my breath.
	- I waited impatiently.
	- I paused.
}
~ counter++
{ counter < 9: -> cycle | -> once }

= once
=== { counter }
{ once:
	- Would my luck hold?
	- Could I win the hand?
}
~ counter++
{ counter < 13 : -> once | -> shuffle }

= shuffle
=== { counter }
{ SEED_RANDOM(counter) }
At the table, I drew a card. <>
{ shuffle:
	- 	Ace of Hearts.
	- 	King of Spades.
	- 	2 of Diamonds.
		'You lose this time!' crowed the croupier.
}
~ counter++
{ counter < 16 : -> shuffle | -> nested }

= nested
=== { counter }
{ SEED_RANDOM(counter) }
{ true:
    { shuffle: 
    - Okay. Ace of Hearts again.
    - Okay. Ace of Hearts?!
    }
}
-> END