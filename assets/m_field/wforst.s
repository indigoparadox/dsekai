
start 0:
   face     east
   sleep    5
   goto     0

interact 0:
   push     5 ; x
   push     1 ; y
   warp     5 ; forst
   return   STACK_RC

