VAR counter = 0
-> coffee

= coffee
{I bought a coffee with my five-pound note.|I bought a second coffee for my friend.|I didn't have enough money to buy any more coffee.}
~ counter++
{ counter < 4 : -> coffee | -> today }

= today
It was {&Monday|Tuesday|Wednesday|Thursday|Friday|Saturday|Sunday} today.
~ counter++
{ counter < 15 : -> today | -> joke}

= joke
He told me a joke. {!I laughed politely.|I smiled.|I grimaced.|I promised myself to not react again.}
~ counter++
{ counter < 20 : -> joke | -> coin}

= coin
{ SEED_RANDOM(counter) }
I tossed the coin. {~Heads|Tails}.
~ counter++
{ counter < 22 : -> coin | -> lights}

= lights
I took a step forward. {!||Then the lights went out. -> dark }
~ counter++
{ counter < 27 : -> lights | -> ratbear}

= dark
So dark...
-> ratbear

= ratbear
The Ratbear {&{wastes no time and |}swipes|scratches } {&at you|into your {&leg|arm|cheek}}.
~ counter++
{ counter < 32 : -> ratbear | -> END }