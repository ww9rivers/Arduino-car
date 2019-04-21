#ifndef _AVOIDANCE_H_
#define _AVOIDANCE_H_   1

/**
 *  Header file for the Object Avoidance module: 
 *  
 *  Only functions, definitions, and objects that we want other modules
 *  to use are exposed here. In this case, only two functions.
 */
#include "Combo.h"

//	Out of range
#define OOR   999

#define MEASURE_LEFT    0
#define MEASURE_FRONT   1
#define MEASURE_RIGHT   2
extern int measuring_pos;
extern int turning_direction;

Op_Mode avoidance_setup(void);
void avoidance_loop(void);
int distance_test(void);
bool measuring_loop(void);
void turn_sensor(void);

#endif