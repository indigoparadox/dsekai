
start 0:
   face        west
   sleep       5
   goto        0

interact 0:
   disable     1
   speak       6 ; "Found an editor!"
   give        1236 ; Editor
   pop         0 ; Items pushed. TODO: GOTO if failed.
   die         0
   return      stack_i ; No RC as a buffer against dupe if there's a bug.

