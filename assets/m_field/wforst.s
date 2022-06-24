
start 0:
   face     east
   sleep    5
   goto     0

interact 0:
   disable  1
   push     5 ; x
   push     1 ; y
   warp     5 ; forst
   disable  0
   return   stack

