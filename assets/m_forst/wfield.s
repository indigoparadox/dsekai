
start 0:
   face     2
   sleep    5
   goto     0

interact 0:
   push     5 ; x
   push     14 ; y
   warp     0 ; field
   disable  0
   return   stack

