
start 0:
   sleep    5
   goto     0

interact 0:
   speak    7 ; "You'll need this hoe to till soil."
   give     1237 ; Hoe
   pop      0 ; Items pushed. TODO: GOTO if failed.
   disable  0
   return   stack_i

interact 1:
   speak    8 ; "Do your best!"
   disable  0
   return   stack

