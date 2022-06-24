
start 0:
   face        east
   push        east
   walk        1
   sleep       3
   push        east
   walk        1
   sleep       9
   face        west
   push        west
   walk        1
   sleep       3
   push        west
   walk        1
   sleep       9
   goto        0 ; Infinite loop.

interact 0:
   disable     1
   speak       0 ; "Try planting this in soil."
   give        1234 ; Turnip seed.
   pop         0 ; Items pushed. TODO: GOTO if failed.
   disable     0
   return      stack_i

interact 1:
   disable     1
   speak       8 ; "Do your best!"
   disable     0
   return      stack

