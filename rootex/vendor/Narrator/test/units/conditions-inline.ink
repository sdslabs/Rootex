VAR mood = 10
VAR midnight = true

-> simple

=== simple
Simple condition: { mood > 20 : Hmm... { mood > 40 : Wow! } I was feeling positive enough. } Okay.
~ mood += 20
{ mood > 50 : -> complex | -> simple }

=== complex
Complex condition with midnight. { midnight : Wow! { midnight : Nice! | Bad! } | { not midnight : Very nice! | Very bad! } This is the end. }
{ midnight != true : -> END }
~ midnight = false
-> complex