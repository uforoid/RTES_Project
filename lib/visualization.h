//------------------------------------------------------------------------------
// VISUALIZATION.C HANDLES THE CALCULUS OF CITY OBJECT POSITIONS AND BEHAVIOUR
// AND CONTAINS THE DISPLAY TASK THAT REFRESHES THE SCREEN
//------------------------------------------------------------------------------
#ifndef VISUALIZATION_H
#define VISUALIZATION_H

/* standard libraries */
#include <stdio.h>
#include <allegro.h>

/* ptask library */
#include "pmutex.h"
#include "ptask.h"
#include "tstat.h"

/* program headers */
#include "../lib/parameters.h"
#include "../lib/cars.h"

/* stores semaphore information */
struct semaphore{
    int x;
    int y;
    int n_sem;
    int color;
};

struct  semaphore s[N_SEMAPHORES];              //saves all semaphores in the city

int     spawn_x[N_SPAWNS], spawn_y[N_SPAWNS];   //x and y of the spawn positions

/* function declaration */
void allegro_initialization();
void city_initialization();
void calculate_spawns();
void calculate_semaphores();
void load_cars();
void close_allegro();

/* task in visualization.c file */
ptask display();
ptask semaphore_task();

#endif //VISUALIZATION_H