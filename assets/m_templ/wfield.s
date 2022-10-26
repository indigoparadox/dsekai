
start 0:
   face     2
   sleep    5
   goto     0

interact 0:
   push     4 ; x
   push     1 ; y
   warp     0 ; field
   disable  0
   return   stack

