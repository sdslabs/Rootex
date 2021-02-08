-> meet_guard

=== meet_guard ===
The guard frowns at you.

* 	(greet) [Greet him]
	'Greetings.'
*	(get_out) 'Get out of my way[.'],' you tell the guard.

- 	'Hmm,' replies the guard.

*	{greet} 	'Having a nice day?' // only if you greeted him

* 	'Hmm?'[] you reply.

*	{get_out} [Shove him aside] 	 // only if you threatened him
	You shove him sharply. He stares in reply, and draws his sword!
	-> fight_guard 			// this route diverts out of the weave

-	'Mff,' the guard replies, and then offers you a paper bag. 'Toffee?'


=== fight_guard ===

-> throw_something

= throw_something 
*	(rock) [Throw rock at guard] -> throw
* 	(sand) [Throw sand at guard] -> throw

= throw
You hurl {throw_something.rock:a rock|a handful of sand} at the guard.