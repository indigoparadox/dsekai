
start 0:
   face        east
   walk_east   1
   sleep       1
   walk_east   1
   sleep       3
   face        west
   walk_west   1
   sleep       1
   walk_west   1
   sleep       3
   goto        0 ; Infinite loop.

interact 0:
   speak       0 ; "Try planting this in soil."
   item_give   1234 ; Turnip seed.
   return      STACK

interact 1:
   speak       8 ; "Do your best!"
   return      STACK_RC

