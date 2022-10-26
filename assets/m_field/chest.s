
start 0:
   face        west
   sleep       5
   goto        0

interact 0:
   speak       6 ; "Found an editor!"
   give        1236 ; Editor
   die         0
   return      stack_i ; No RC as a buffer against dupe if there's a bug.

