CONST LOBBY = 1
CONST STAIRCASE = 2
CONST HALLWAY = 3

CONST HELD_BY_AGENT = -1

VAR secret_agent_location = LOBBY
VAR suitcase_location = HALLWAY

-> report_progress

=== report_progress ===
{  secret_agent_location == suitcase_location:
	The secret agent grabs the suitcase!
	~ suitcase_location = HELD_BY_AGENT  
	
-  secret_agent_location < suitcase_location:
	The secret agent moves forward.
	~ secret_agent_location++
	-> report_progress
}