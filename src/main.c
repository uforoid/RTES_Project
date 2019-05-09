#include <allegro.h>

#include "../lib/visualization.h"
#include "../lib/cars.h"
#include "../lib/parameters.h"

int main(void){

    ptask_init(SCHED_RR, GLOBAL, NO_PROTOCOL);

    /* calculate initial position information of city elements */
    allegro_initialization();
    city_initialization();
    calculate_spawns();
    calculate_semaphores();

    /* loading cars images */
    load_cars();

    /* creation of all the tasks */
    create_tasks();

    /* wait a key press to terminate the program */
    close_allegro();
}