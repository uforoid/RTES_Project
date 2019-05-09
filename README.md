# ProgettoRTOS
City. ​Simulate ​an ​urban ​area ​with ​a number ​of crossing ​streets ​with ​traffic lights
managed ​by a single traffic control task. Cars are randomly generated to enter and
exit the​ area. Each car is autonomous and is controlled by a periodic task based on
local sensors.

The city building blocks are green and in the middle of the street at the crossings
there are the semaphores.

it's important to change the parameters in /lib/parameters.h to match the size of your screen

cars city driving processing application:\
mkdir build\
cd build\
cmake ..\
make\
sudo ./application

Extern Dependences:

sudo apt-get install git\
sudo apt-get install build-essential\
sudo apt-get install cmake\
sudo apt-get install liballegro4.2-dev\
sudo apt-get install pkg-config

//-------------------------------------------------------------------------------------

Ptask:

git clone https://github.com/glipari/ptask.git \
mkdir build\
cd build\
cmake ..\
make\
make install

/* 
  _(PTASK ISSUE)_
  In order to compile and install ptask with allegro comment inside CMakeLists.txt the row
  add_subdirectory(examples)
  and delete these 3 lines in ptask/src/CMakeLists.txt
  if(TRACE)
   tpt_provider.h
  endif()
*/
