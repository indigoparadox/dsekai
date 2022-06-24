
start 0:
   sleep    5
   goto     0

interact 0:
   disable  1
   speak    2 ; "It's a fountain."
   disable  0
   return   stack

