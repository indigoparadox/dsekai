
start 0:
   push        random
   face        stack_p
   walk        stack
   sleep       3
   goto        0 ; Infinite loop.

interact 0:
   speak       1 ; Not yeti...
   return      stack

