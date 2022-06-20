
start 0:
   face        west
   sleep       5
   goto        0

interact 0:
   speak       6 ; "Found an editor!"
   item_give   1236 ; Editor
   die         0
   return      STACK

