LIST DoctorsInSurgery = Adams, Bernard, (Cartwright), Denver, Eamonn 

Cartwright == { DoctorsInSurgery }

~ DoctorsInSurgery = (Adams, Bernard)
Adams, Bernard == { DoctorsInSurgery }

~ DoctorsInSurgery = ()
Empty == { DoctorsInSurgery } Empty

~ DoctorsInSurgery = DoctorsInSurgery + Adams
Adams == { DoctorsInSurgery }

~ DoctorsInSurgery += Eamonn
Adams, Eamonn == { DoctorsInSurgery }

~ DoctorsInSurgery -= Eamonn 
Adams == { DoctorsInSurgery }

~ DoctorsInSurgery += (Eamonn, Denver) 
Adams, Eamonn, Denver == { DoctorsInSurgery }

~ DoctorsInSurgery -= (Adams, Eamonn, Denver)
Empty == { DoctorsInSurgery } Empty again!

...
~ DoctorsInSurgery = (Adams, Cartwright)
{LIST_COUNT(DoctorsInSurgery)} doctors in surgery.
First is {LIST_MIN(DoctorsInSurgery)}.
Last is {LIST_MAX(DoctorsInSurgery)}.

Random seed = 1 { SEED_RANDOM(1)}
A random doctor: {LIST_RANDOM(DoctorsInSurgery)}

Working time! { DoctorsInSurgery: The surgery is open today. | Everyone has gone home. }
~ DoctorsInSurgery = ()
It's to late. { DoctorsInSurgery: The surgery is open today. | Everyone has gone home. }

~ DoctorsInSurgery = (Adams, Bernard)
{ DoctorsInSurgery == (Adams, Bernard):
	Dr Adams and Dr Bernard are having a loud argument in one corner.
}

{ DoctorsInSurgery ? (Adams, Bernard):
	Dr Adams and Dr Bernard are having a hushed argument in one corner.
}

~ DoctorsInSurgery += Eamonn

{ DoctorsInSurgery != (Adams, Bernard):
	At least Adams and Bernard aren't arguing. 
}

{ DoctorsInSurgery has Eamonn:
	Dr Eamonn is polishing his glasses.
}

{ DoctorsInSurgery !? (Adams, Bernard) : Yeap, Adams and Bernard are outside. | Nope, Adams and Bernard are here. }

All the doctors ({LIST_COUNT(LIST_ALL(DoctorsInSurgery))}): { LIST_ALL(DoctorsInSurgery) }
All the doctors again: { LIST_ALL(Adams) } 

VAR myList = ()
~ myList = DoctorsInSurgery()
~ myList += Adams
{ myList } here!