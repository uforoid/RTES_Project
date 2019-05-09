#include "../lib/visualization.h"

/*
________________________________________________________________________________

	        HANDLING OF GRAPHIC PROPERITES AND DISPLAY TASK
________________________________________________________________________________
*/

/* 
it's possible to choose the functioning mode of the semaphores

    ONE_SEMAPHORE: only one semaphore active at a time
    TWO_SEMAPHORE: two semaphores active at a time

semaphore period must be changed separately in the parameters.h file
*/

//#define ONE_SEMAPHORE
#define TWO_SEMAPHORE

BITMAP*     cars_img[MAX_CARS];		//cars loaded in memory
BITMAP*     my_screen;				//screen buffer
BITMAP*     city;					//contains city scheme

/* allegro library initialization */
void allegro_initialization(){

    allegro_init();
    install_keyboard();

    set_color_depth(16);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, MY_SCREEN_W, MY_SCREEN_H, 0, 0);
    clear_to_color(screen, 0);
}

/* loading car images in memory */
void load_cars(){
	int i, j;							//indices
    char *file_names[N_SAVED_CARS];		//names of car image files
    BITMAP *temporary_cars[MAX_CARS];	//cars at original dimensions

    file_names[0] = "../media/car1.bmp";
    file_names[1] = "../media/car2.bmp";
    file_names[2] = "../media/car3.bmp";
    j = 0;

	//loading images
    for(i = 0; i<MAX_CARS; i++){
        temporary_cars[i] = load_bitmap(file_names[j], NULL);
        j = (j + 1) % N_SAVED_CARS;
    }

    //reshaping of images
    for(i = 0; i<MAX_CARS; i++){
        temporary_cars[i] = load_bitmap(file_names[j], NULL);

        cars_img[i] = create_bitmap((int)CAR_LENGTH, (int)CAR_WIDTH);

        stretch_blit(temporary_cars[i], cars_img[i],
            0, 0, temporary_cars[i]->w, temporary_cars[i]->h, 
            0, 0, cars_img[i]->w, cars_img[i]->h);
        
        destroy_bitmap(temporary_cars[i]);
        j = (j + 1) % N_SAVED_CARS;
    }
}

/* printing car in buffer at position t_x and t_y */
static void display_car(int t_x, int t_y, int rot, int car){
    int 	x, y;	//position of the car

    x = t_x - cars_img[car]->w/2;
    y = t_y - cars_img[car]->h/2;
    rotate_sprite(my_screen, cars_img[car], x, y, itofix(rot));
}

/* calculate building position */
void city_initialization(){
    int     i, j;				//indices
    int     init_x, init_y;		//initial x and y of building
	int		end_x, end_y;		//finale x and y of building

    city = create_bitmap(MY_SCREEN_W, MY_SCREEN_H);

    for(i=0; i<CITY_BLOCKS; i++){
        for (j=0; j<CITY_BLOCKS; j++){

            init_x = CITY_START_X + i * (BLOCKS_H + STREET_H);
            init_y = CITY_START_Y + j * (BLOCKS_H + STREET_H);
            end_x = CITY_START_X + i * (BLOCKS_H + STREET_H) + BLOCKS_H;
            end_y = CITY_START_Y + j * (BLOCKS_H + STREET_H) + BLOCKS_H;

            rectfill(city, init_x, init_y, end_x, end_y, GREEN); 
        }
    }
}

/* calculate spawn position */
void calculate_spawns(){
    const int 	spawn_lato = N_SPAWNS / 4;					//spawn side
    int 		i, j;										//indices
    int 		s_x[CITY_BLOCKS + 1], s_y[CITY_BLOCKS + 1]; //spawn positions

    //calculate spawn x and y
    for(i = 0; i < CITY_BLOCKS + 1; i++){
        s_x[i]= CITY_START_X + i * (BLOCKS_H + STREET_H/2) + (i-1) * STREET_H/2;
        s_y[i]= CITY_START_Y + i * (BLOCKS_H + STREET_H/2) + (i-1) * STREET_H/2;
    }

    //left side
    for(i = 0; i < spawn_lato; i++){
        spawn_x[i] = s_x[0];
        spawn_y[i] = s_y[i + 1] + STREET_H / 4;
    }

    //up side
    for(i = 0; i < spawn_lato; i++){
        spawn_x[spawn_lato + i] = s_x[i + 1] - STREET_H / 4;
        spawn_y[spawn_lato + i] = s_y[0];
    }

    //right side
    for(i = 0; i < spawn_lato; i++){
        spawn_x[spawn_lato*2 + i] = s_x[CITY_BLOCKS];
        spawn_y[spawn_lato*2 + i] = s_y[i + 1] - STREET_H / 4;
    }

    //down side
    for(i = 0; i < spawn_lato; i++){
        spawn_x[spawn_lato*3 + i] = s_x[i + 1] + STREET_H / 4;
        spawn_y[spawn_lato*3 + i] = s_y[CITY_BLOCKS];
    }
}

/* calculate semaphore position */
void calculate_semaphores(){
	const int 	n_cross = (CITY_BLOCKS-1) * (CITY_BLOCKS-1);	//crossing numbers
    int 		i, j;											//indices
    int 		t_x[n_cross], t_y[n_cross];						//temporary x and y
    int 		cross_x[n_cross], cross_y[n_cross];				//crossing x and y

    for(i = 0; i < CITY_BLOCKS - 1; i++){
        t_x[i]= CITY_START_X + (i+1) * (BLOCKS_H + STREET_H/2) + i * STREET_H/2;
        t_y[i]= CITY_START_Y + (i+1) * (BLOCKS_H + STREET_H/2) + i * STREET_H/2;
    }

    for(i = 0; i < CITY_BLOCKS - 1; i++){
        for(j = 0; j < CITY_BLOCKS - 1; j++){
            cross_x[i*(CITY_BLOCKS - 1) + j] = t_x[i];
            cross_y[i*(CITY_BLOCKS - 1) + j] = t_y[j];
        }
    }

    for(i = 0; i < n_cross; i++){
        //Semaforo a sinistra
        s[i * (CITY_BLOCKS - 1)].x = cross_x[i] - STREET_H/2;
        s[i * (CITY_BLOCKS - 1)].y = cross_y[i] + STREET_H/4;
        s[i * (CITY_BLOCKS - 1)].n_sem = 0;
        s[i * (CITY_BLOCKS - 1)].color = RED;

        //Seamforo in alto
        s[i * (CITY_BLOCKS - 1) + 1].x = cross_x[i] - STREET_H/4;
        s[i * (CITY_BLOCKS - 1) + 1].y = cross_y[i] - STREET_H/2;
        s[i * (CITY_BLOCKS - 1) + 1].n_sem = 0;
        s[i * (CITY_BLOCKS - 1) + 1].color = RED;

        //Semaforo a destra
        s[i * (CITY_BLOCKS - 1) + 2].x = cross_x[i] + STREET_H/2;
        s[i * (CITY_BLOCKS - 1) + 2].y = cross_y[i] - STREET_H/4;
        s[i * (CITY_BLOCKS - 1) + 2].n_sem = 1;
        s[i * (CITY_BLOCKS - 1) + 2].color = RED;

        //Seamforo in basso
        s[i * (CITY_BLOCKS - 1) + 3].x = cross_x[i] + STREET_H/4;
        s[i * (CITY_BLOCKS - 1) + 3].y = cross_y[i] + STREET_H/2;
        s[i * (CITY_BLOCKS - 1) + 3].n_sem = 1;
        s[i * (CITY_BLOCKS - 1) + 3].color = RED;
    }
}

/* printing semaphores on buffer */
static void display_semaphores(){
    int 	i;	//index
    
    for(i = 0; i < N_SEMAPHORES; i++){
        rectfill(my_screen, s[i].x - SEMAPHORE_H / 2, s[i].y - SEMAPHORE_H / 2, 
            s[i].x + SEMAPHORE_H / 2, s[i].y + SEMAPHORE_H / 2, s[i].color);
    }
}

/* semaphores task */
ptask semaphore_task(){
    int 	i, j;	//indices
	int 	side;	//semaphore side

    side = j = 0;

	/* choosing semaphore mode */

	#ifdef ONE_SEMAPHORE

    while(1){
        pthread_mutex_lock(&mutex);
        if (side > 3) side = 0;

        for(i = 0; i < N_SEMAPHORES; i++){
            if (i % 4 == side)
                s[i].color = GREEN;
            else
                s[i].color = RED;
            
        }
        side++;

        pthread_mutex_unlock(&mutex);
        ptask_wait_for_period();
    }

	#endif //ONE_SEMAPHORE

	#ifdef TWO_SEMAPHORE

    while(1){
        pthread_mutex_lock(&mutex);
        if (j == 6){
            j = 0;
            if (side == 1) side = 0;
            else side = 1;
        }
        if (j < 2){
            for(i = 0; i < N_SEMAPHORES; i++){
                if (i % 2 == side && s[i].n_sem == j)
                    s[i].color = GREEN;

                if (i % 2 != side)
                    s[i].color = RED;
            }
        }
        j++;
        pthread_mutex_unlock(&mutex);
        ptask_wait_for_period();
    }

	#endif //TWO_SEMAPHORE
}

/* display task, prints buffer on the screen */
ptask display(){
    int 	i;	//index

    my_screen = create_bitmap(MY_SCREEN_W, MY_SCREEN_H);

    //drawing cycle, prints city, semaphore and cars
    while(!key[KEY_ESC]){
        blit(city, my_screen, 0, 0, 0, 0, city->w, city->h);
        display_semaphores();

        pthread_mutex_lock(&mutex);
        for(i = 0; i < MAX_CARS; i++){
            if (g[i].active == 1){
               display_car(g[i].x, g[i].y, g[i].rot, g[i].car);
            }
        }
        pthread_mutex_unlock(&mutex);
        
        blit(my_screen, screen, 0, 0, 0, 0, screen->w, screen->h);
        ptask_wait_for_period();
    }
}

/* handles program closure */
void close_allegro(){

    readkey();
    allegro_exit();
}