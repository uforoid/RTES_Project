#include "../lib/cars.h"

#define PI 3.14159265

/*
________________________________________________________________________________

		        HANDLING OF CARS CREATION AND MOVEMENT
________________________________________________________________________________
*/

int		active_cars; 	//number of active cars in the city

/* car struct initialization  */
void g_car_init(int car, int x, int y, int rot, int vmax){   
    
    //90 degrees become 64 in allegro degrees
    g[car].rot = g[car].rot_i = rot * 64; 
    g[car].car = car;
    g[car].x = g[car].x_i = x;
    g[car].y = g[car].y_i = y;
    g[car].vx = g[car].vx_i = 0;
    g[car].vy = g[car].vy_i = 0;
    g[car].ptask = -1;
    g[car].active = -1;

    //velocity depends on the initialization direction
    if (rot == 0)
        g[car].vx = g[car].vx_i = VELOCITY;
    if (rot == 1)
        g[car].vy = g[car].vy_i = VELOCITY;
    if (rot == 2)
        g[car].vx = g[car].vx_i = -VELOCITY;
    if (rot == 3)
        g[car].vy = g[car].vy_i = -VELOCITY;
}

/* restore car's initial state  */
void reset_car(int car){
    g[car].rot = g[car].rot_i;
    g[car].x = g[car].x_i;
    g[car].y = g[car].y_i;
    g[car].vx = g[car].vx_i;
    g[car].vy = g[car].vy_i;
    g[car].active = -1;

    active_cars--;
}

/* return +1 or -1 depending on number sign */
int sgn(int x){
    int 	r;	//return value

	r = 0;
    if (x < 0) r = -1;
    if (x > 0) r = +1;
    return r;
}

void unlock_and_wait(){
    pthread_mutex_unlock(&mutex);
    ptask_wait_for_period();
}

/* returns distance of the two points given */
float distance(int x1, int y1, int x2, int y2){
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

/* returns 1 if the coordinates are out of the city perimeter */
int out_of_bound(int x, int y){
    int		out;	//boolean, 1 if outside the city margins
    
    out = 0;
    if (x > CITY_END_X + STREET_H / 2 || x < CITY_START_X - STREET_H / 2)
        out = 1;
    if (y > CITY_END_Y + STREET_H / 2 || y < CITY_START_Y - STREET_H / 2)
    	out = 1;
    
    return out;
}

/* returns 1 if the car is coing to collide with another car */
int collision(int car){
    int		i;					//index
    int		x, y;				//testing coordinates
    int		collision;			//boolean, 1 if there is collision
    float	front_x, front_y;	//front coordinates of the car
    double	r_angle;			//angle in radiants
	int 	step;				//maximum movement of the car

    collision = 0;
    r_angle = fixtof(itofix(g[car].rot)) * PI / 128;
	step = ceil(VELOCITY * V_PENALTY);

    front_x = g[car].x + cos(r_angle) * (CAR_LENGTH / 2 + 3);
    front_y = g[car].y + sin(r_angle) * (CAR_LENGTH / 2 + 3);
    
    for(i = 0; i < step; i++){
        x = ceil(front_x + i * cos(r_angle));
        y = ceil(front_y + i * sin(r_angle));

        if(getpixel(screen, x, y) != RED &&
            getpixel(screen, x, y) != GREEN &&
            getpixel(screen, x, y) != BLACK){
            collision = 1;
        }
    }
    return collision;
}

/* check if there is a semaphore and saves the coordinates */
int semaphores(int car, int* sem_x, int* sem_y){
    int		i;				//index
    int		sem;			//boolean, 1 if there is a semaphore
    int		step;			//maximum movement of the car
	int		exit_next;		//boolean of the next cycle
    int		sgn_x, sgn_y;	//sign of vx and vy

    sem = 0;
	exit_next = 1;
	sgn_x = sgn(g[car].vx);
	sgn_y = sgn(g[car].vy);
    step = ceil(VELOCITY * V_PENALTY);

    *sem_x = g[car].x + sgn_x * (STREET_H / 2);
    *sem_y = g[car].y + sgn_y * (STREET_H / 2);

    for(i = 0; i < step && sem == 0; i++){
        *sem_x += sgn_x;
        *sem_y += sgn_y;

        if(getpixel(screen, *sem_x, *sem_y) == RED){
			sem = 1;
            while(exit_next){

				if(getpixel(screen, *sem_x, *sem_y) != RED){
					*sem_x = *sem_x - sgn_x * SEMAPHORE_H / 2;
					*sem_y = *sem_y - sgn_y * SEMAPHORE_H / 2;
					exit_next = 0;
				}
				else{
				*sem_x +=sgn_x;
        		*sem_y +=sgn_y;
				}
			}
        }
    }
    return sem;
}

/* returns only if the semaphore turned green */
void wait_semaphore(int car){
    int     tmp_x, tmp_y;   //needed to call the function semaphores()
    int     exit_next;      //boolean for the cycle

    exit_next = 1;
    while(exit_next){
        pthread_mutex_lock(&mutex);

        tmp_x = g[car].x;
        tmp_y = g[car].y;

        if(semaphores(car, &tmp_x, &tmp_y) == 0){
            exit_next = 0;
        }

        unlock_and_wait();
    }
}

/* returns only when the car is at the center of the semaphore */
void get_to_cross(int car, int sem_x, int sem_y){
    float   step;       //maximum movement of the car
    int     exit_next;  //boolean for the cycle
    
    step = VELOCITY * V_PENALTY;
    
    exit_next = 1;
    while (exit_next){
        pthread_mutex_lock(&mutex);

        if (!collision(car)){
            g[car].x+= (int) g[car].vx * V_PENALTY;
            g[car].y+= (int) g[car].vy * V_PENALTY;

            if (distance(g[car].x, g[car].y, sem_x, sem_y) < step)
                exit_next = 0;
        }
        unlock_and_wait();
    }
}

//------------------------------------------------------------------------------
// manages car rotation at the semaphore, needs to be called with
// the position given by the function semaphores()
//------------------------------------------------------------------------------
void turn(int car, int dir, int sem_x, int sem_y){
    
    int     i;					//index
	int 	rot_i;				//initial rotation
    int     sgn_x, sgn_y;		//sign of vx and vy
	int 	exit_next;			//boolean for the cycle
    float   x_i, y_i;			//initial position of the car
	float 	angle;				//actual car rotation 
    float   delta;				//movement depending on car direction rotation
	float 	delta_x, delta_y;	//variation of x and y position
    float   delta_rot;			//rotation variation

    //getting to the center of the semaphore
    wait_semaphore(car);
    get_to_cross(car, sem_x, sem_y);

    delta_rot = VELOCITY * V_PENALTY / 1.2;
    sgn_x = sgn(g[car].vx);
	sgn_y = sgn(g[car].vy);
	rot_i = g[car].rot;

    if(dir == -1) delta = ( 3 * STREET_H / 4.0);
    if(dir == +1) delta = (STREET_H / 4.0);

    pthread_mutex_lock(&mutex);
    g[car].x = x_i = (float)sem_x;
    g[car].y = y_i = (float)sem_y;
    pthread_mutex_unlock(&mutex);

    //setting variable depending on rotation
    if(g[car].vx != 0){
        delta_x = sgn_x * delta;
        delta_y = dir * sgn_x * delta;
		g[car].vy = dir * g[car].vx;
        g[car].vx = 0;
    }
    else{
        delta_x = -dir * sgn_y * delta;
        delta_y = sgn_y * delta;
		g[car].vx= -dir * g[car].vy;
        g[car].vy = 0;
    }

    //turning cycle
    angle = 0;
    exit_next = 1;
    while(exit_next){
        pthread_mutex_lock(&mutex);
        if (angle >= 64){
            exit_next = 0;
            angle = 64;
        }

        if (!collision(car)){
            g[car].rot = rot_i + angle * dir;
            g[car].x = x_i + delta_x * angle / 64;
            g[car].y = y_i + delta_y * angle / 64;
            angle+= delta_rot;
        }

        unlock_and_wait();
    }
}

/* main task that handles car movement */
ptask car_thread(void){
    int 	i, j;			//indices
	int 	car;			//number of car in the array
    int 	sem_x, sem_y;	//semaphore position if there is one
    int 	dir;			//turning direction
	int		index;			//ptask index

    //saving task index
    index = ptask_get_index();
    pthread_mutex_lock(&mutex);
    for(i = 0; i < MAX_CARS; i++){
        if (g[i].ptask == index)
            car = i;
    }
    pthread_mutex_unlock(&mutex);

    //movement cycle
    while(1){
        pthread_mutex_lock(&mutex);

        if(g[car].active == -1 || collision(car)){
            unlock_and_wait();
        }
        else if (semaphores(car, &sem_x, &sem_y)){
            dir = rand() % 2;
            if (dir == 0) dir = -1;
            pthread_mutex_unlock(&mutex);
            turn(car, dir, sem_x, sem_y);
        }
        else{
            g[car].x += g[car].vx * V_PENALTY;
            g[car].y += g[car].vy * V_PENALTY;

            if (out_of_bound(g[car].x, g[car].y))
                reset_car(car);

            unlock_and_wait();
        }
    }
}

/* task that activates the cars */
ptask spawn_task(){
	int 	car;	//index of car to wake up

    active_cars = 0;

    //activation cycle car
    while(1){
        pthread_mutex_lock(&mutex);
        while (active_cars >= MAX_CARS){
            unlock_and_wait();
            pthread_mutex_lock(&mutex);
        }

        car = rand() % MAX_CARS;
        while (g[car].active != -1){
            car = rand() % MAX_CARS;
        }

        g[car].active = 1;
        ptask_activate(g[car].ptask);
        active_cars++;

        unlock_and_wait();
    }

}

//------------------------------------------------------------------------------
// called by main, initializes all car structure and handles
// the creation of all the tasks in the program
//------------------------------------------------------------------------------
void create_tasks(){
    
    int 	i, j;		//indices
    int 	rotation;	//rotation of the car

    pmux_create_pi(&mutex);
	srand(time(NULL));
    printf("\n");
	j = 0;

    pthread_mutex_lock(&mutex);
    for(i = 0; i < MAX_CARS; i++){
        rotation = (i / (CITY_BLOCKS - 1));
        g_car_init(i, spawn_x[i], spawn_y[i], rotation, VELOCITY);
		
		g[i].ptask = ptask_create_prio(car_thread, C_PERIOD, C_PRIORITY - i, NOW);
        if (g[i].ptask == -1){
            printf("\n Error in the creation of the task %d\n", i);
        }
        j = (j + 1) % N_SAVED_CARS;
    }

    ptask_create_prio(display, D_PERIOD, D_PRIORITY, NOW);
    ptask_create_prio(spawn_task, SP_PERIOD, SP_PRIORITY, NOW);
    ptask_create_prio(semaphore_task, SE_PERIOD, SE_PRIORITY, NOW);
    pthread_mutex_unlock(&mutex);
}