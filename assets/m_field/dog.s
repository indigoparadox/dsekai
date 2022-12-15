
start 0:
   face        follow
   push        follow
   walk        1
   sleep       3
   goto        0 ; Infinite loop.

interact 0:
   speak       9 ; "Woof! Woof!"
   disable     0
   return      stack

