
start 0:
   face     east
   sleep    5
   goto     0

interact 0:
   disable  1
   push     14 ; x
   push     38 ; y
   warp     4 ; templ
   disable  0
   return   stack

