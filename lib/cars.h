//------------------------------------------------------------------------------
// CARS.C GESTISCE LA CREAZIONE DI TUTTI I TASK DEL PROGRAMMA E CI SONO LE
// FUNZIONI E I TASK CHE GESTISCONO IL MOVIMENTO DELLE MACCHINE
//------------------------------------------------------------------------------
#ifndef CARS_H
#define CARS_H

/* standard libraries */
#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

/* ptask library */
#include "pmutex.h"
#include "ptask.h"
#include "tstat.h"

/* program headers */
#include "../lib/parameters.h"
#include "../lib/visualization.h"

/* stores all car information */
struct gestore_car{
    float x, y;
    float x_i, y_i;
    int car;
    int rot, rot_i;
    float vx, vy;
    float vx_i, vy_i;
    int ptask;
    int active;
};

struct gestore_car  g[MAX_CARS];    //array that saves all the cars
pthread_mutex_t     mutex;          //mutex of all the program

void create_tasks();

#endif //CARS_H