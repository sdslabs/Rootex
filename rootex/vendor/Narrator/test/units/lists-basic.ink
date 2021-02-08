LIST daysOfTheWeek = Monday, Tuesday, Wednesday, Thursday, Friday 
VAR today = Monday
VAR tomorrow = Tuesday

LIST heatedWaterStates = cold, boiling, recently_boiled
VAR kettleState = cold 
VAR potState = cold 

LIST colours = red, green, blue, purple 
LIST moods = mad, happy, blue
VAR status = colours.blue

LIST volumeLevel = off, quiet, medium, loud, deafening
VAR lecturersVolume = quiet
VAR murmurersVolume = quiet

LIST Numbers = one, two, three 
VAR cats = one

~ cats = Numbers(2) // score will be "two"
We have { cats } cats.

-> today
== today

Today is { today }. Tomorrow is { tomorrow }.

...
-> kitchen
=== kitchen


- Hm, kettle is { kettleState } and pot is { potState }.
~ kettleState = boiling
- Now kettle is { kettleState } and pot is { potState }.

...
-> status
=== status

Binary values are { status == colours.blue } and { status == moods.blue }

...
-> lecture
=== lecture

{ lecturersVolume < deafening:
	~ lecturersVolume++
    The lecturer's voice becomes {lecturersVolume}.
	The lecturer has {LIST_VALUE(deafening) - LIST_VALUE(lecturersVolume)} notches still available to him.

	{ lecturersVolume > murmurersVolume:
		~ murmurersVolume++
		The murmuring gets louder.
	}
    -> lecture
}