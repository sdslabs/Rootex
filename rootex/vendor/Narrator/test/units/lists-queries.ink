LIST GuardsOnDuty = (Smith), (Jones), Carter, Braithwaite
LIST CoreValues = strength, courage, compassion, greed, nepotism, delusions_of_godhood

Normal: { GuardsOnDuty }
~ GuardsOnDuty = LIST_INVERT(GuardsOnDuty)
Inverted: { GuardsOnDuty }

Range: { LIST_RANGE(GuardsOnDuty, Jones, Carter) }

...
VAR desiredValues = (strength, courage, compassion, nepotism )
VAR actualValues =  ( greed, nepotism, delusions_of_godhood )
{desiredValues ^ actualValues: The new president has at least one desirable quality. A cold {desiredValues ^ actualValues}.}
{LIST_COUNT(desiredValues ^ actualValues) == 1: Correction, the new president has only one desirable quality. {desiredValues ^ actualValues == nepotism: It's the scary one.}}

...
LIST Characters = Alfred, Batman, Robin 
LIST Props = champagne_glass, newspaper 

VAR BallroomContents = (Alfred, Batman, newspaper) 
VAR HallwayContents = (Robin, champagne_glass) 
VAR roomState = HallwayContents

-> room
== room
{ roomState ? Alfred: Alfred is here, standing quietly in a corner. } { roomState ? Batman: Batman's presence dominates all. } { roomState ? Robin: Robin is all but forgotten. }
	<> { roomState ? champagne_glass: A champagne glass lies discarded on the floor. } { roomState ? newspaper: On one table, a headline blares out WHO IS THE BATMAN? AND *WHO* IS HIS BARELY-REMEMBERED ASSISTANT? }

{ roomState == BallroomContents :
    -> letters
- else:
    ~ roomState = BallroomContents
    -> room
}

== letters
LIST Letters = a,b,c 
LIST Numbers = one, two, three 
VAR mixedList = (a, three, c)

...
{LIST_ALL(mixedList)}       // a, one, b, two, c, three
{LIST_COUNT(mixedList)}     // 3 
{LIST_MIN(mixedList)}       // a
{LIST_MAX(mixedList)}       // c
{mixedList ? (a,b) }        // 0 (false)
{mixedList ^ LIST_ALL(a)}   // a, c
{ mixedList >= (one, a) }   // 1 (true)
{ mixedList > (three) }     // 0 (false)
{ LIST_INVERT(mixedList) }  // one, b, two