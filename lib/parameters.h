#ifndef PARAMETERS_H
#define PARAMETERS_H

/* screen size */
#define MY_SCREEN_W     2200
#define MY_SCREEN_H     1466

/* city parameters */
#define CITY_BLOCKS     5
#define CITY_START_X    100
#define CITY_START_Y    100

#define BLOCKS_H        200
#define STREET_H        60      //must be even
#define SEMAPHORE_H     10

/* period and priority task properties*/

//DISPLAY
#define D_PERIOD		16
#define D_PRIORITY		80

//CARS
#define C_PERIOD		16
#define C_PRIORITY		70

//SPAWNS
#define SP_PERIOD		2 * 1000 //2 seconds
#define SP_PRIORITY		70

//SEMAPHORES
#define SE_PERIOD		1 * 1000 //1 second
#define SE_PRIORITY		70

/* car parameters*/
#define VELOCITY		10
#define N_SAVED_CARS	3

/* colors */
#define WHITE			makecol(255, 255, 255)
#define RED				makecol(255, 0, 0)
#define GREEN			makecol(0, 255, 0)
#define BLUE			makecol(0, 0, 255)
#define BLACK			makecol(0, 0, 0)

/* city variables calculus */
#define CITY_END_X CITY_START_X + CITY_BLOCKS * (BLOCKS_H + STREET_H) - STREET_H
#define CITY_END_Y CITY_START_Y + CITY_BLOCKS * (BLOCKS_H + STREET_H) - STREET_H

#define N_SPAWNS		((CITY_BLOCKS - 1) * 4)
#define N_SEMAPHORES	((CITY_BLOCKS - 1) * (CITY_BLOCKS - 1) * 4)
#define N_HOMES			(CITY_BLOCKS * CITY_BLOCKS)

/* car variables calculus */
#define MAX_CARS		((CITY_BLOCKS - 1) * 4)

#define CAR_LENGTH		(STREET_H * 0.9)
#define CAR_WIDTH		(STREET_H / 2 * 0.9)

//penalty to car's movement given by the period
#define V_PENALTY		((float)C_PERIOD / 100)

#endif //PARAMETERS_H