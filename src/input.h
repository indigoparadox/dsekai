
#ifndef INPUT_H
#define INPUT_H

#ifdef __DOS__
#include "input/dosi.h"
#else
#include "input/sdli.h"
#endif /* __DOS__ */

int input_poll();

#endif /* INPUT_H */

