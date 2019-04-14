#ifndef _AVOIDANCE_H_
#define _AVOIDANCE_H_   1

/**
 *  Header file for the Object Avoidance module: 
 *  
 *  Only functions, definitions, and objects that we want other modules
 *  to use are exposed here. In this case, only two functions.
 */
#include "Combo.h"

Op_Mode avoidance_setup(void);
void avoidance_loop(void);
#endif