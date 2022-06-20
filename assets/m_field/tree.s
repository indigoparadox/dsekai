
start 0:
   face     north
   sleep    5
   goto     0

interact 0:
   speak       3 ; "Found a fruit!"
   item_give   1235 ; Apple
   die         0
   return      STACK ; No RC as a buffer against dupe if there's a bug.

