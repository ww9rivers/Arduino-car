#ifndef _AVOIDANCE_H_
#define _AVOIDANCE_H_   1

/**
 *  Header file for the Object Avoidance module: 
 *  
 *  Only functions, definitions, and objects that we want other modules
 *  to use are exposed here. In this case, only two functions.
 */
#include "Combo.h"

#define OUT_OF_RANGE          999
#define SENSOR_TURNING        1000
#define object_in_range(x)    ((x)<50)
#define object_near(x)        ((x)<20)
#define object_outof_range(x) ((x)==OUT_OF_RANGE)
#define sensor_not_ready(x)   ((x)==SENSOR_TURNING)

#define MEASURE_LEFT    0
#define MEASURE_FRONT   1
#define MEASURE_RIGHT   2
extern int measuring_pos;
extern int turning_direction;

void avoidance_setup(void);
void avoidance_loop(void);
int distance_average(int count);
int distance_scan(void);
int distance_test(void);
void set_sensor(int sdir);
void turn_sensor(void);

#define distance_avg()  distance_average(3)

#endif
