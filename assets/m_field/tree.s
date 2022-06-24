
start 0:
   face     north
   sleep    5
   goto     0

interact 0:
   speak       3 ; "Found a fruit!"
   give        1235 ; Apple
   pop         0 ; Items pushed. TODO: GOTO if failed.
   die         0
   return      stack_i ; No RC as a buffer against dupe if there's a bug.

