#include <cstdio>
#include <cstdlib>
#if TASK == 4
#include <iostream>
#include <stdint.h>
#include <pthread.h>
#include <vector>
#include <chrono>
#include <thread>
#endif
using namespace std;


#define IMG_SIZE 4*2
#define TASK 1
#define NUM_THREADS 8

#if TASK==1
void brighten_image(char* pixels){
	printf("Task1 starts\n");
	for(int i=0; i<IMG_SIZE; i++){
		*(pixels++) += 9; 
	}
}
#elif TASK==2
void brighten_image(char* pixels){
	printf("Task2 starts\n");
	*(pixels++) += 9;
	*(pixels++) += 9;
	*(pixels++) += 9;
	*(pixels++) += 9;
	*(pixels++) += 9;
	*(pixels++) += 9;
	*(pixels++) += 9;
	*(pixels++) += 9;
}

#elif TASK>=3
void brighten_image(char* pixels){
	printf("Task3 starts\n");
	
	//16 bits at the time
	int16_t *char2 = reinterpret_cast<int16_t*>(pixels);
	*(char2++) += (0x0909);
	*(char2++) += (0x0909);
	*(char2++) += (0x0909);
	*(char2) += (0x0909);
	
	//32 bits at the time
	int32_t *char4 = reinterpret_cast<int32_t*>(pixels);
	*(char4++) += (0x09090909);
	*(char4) += (0x09090909);
	
	//64 bits at the time
	int64_t *char8 = reinterpret_cast<int64_t*>(pixels);
	*(char8) += (0x0909090909090909);
}
#endif


/*
Task4 D with SPMD

spmd_function add_two(char pixels*) {
   i = item_id();
   *(pixels+i) += 9;
}
*/

//Task 4 D custom item_id function and global
#if TASK == 4
int tid = 0;
int item_id(void){
	return tid++;
}

void add_two(char* pixels){
	char *r = reinterpret_cast<const char*>(pixels);
   *(r+item_id()) += 9;
}
#endif

int main (){
	char img[IMG_SIZE+1] = {0,0,0,0,0,0,0,0};

#if TASK<=3
	brighten_image(img);
#elif TASK == 4
	printf("Task4 Starts\n"); 
	std::thread threads[NUM_THREADS];
	std::vector<std::thread> workers;
	for (int i=0;i<NUM_THREADS;i++){
		workers.push_back(std::thread(add_two,img));
	}
	for (int i=0;i<NUM_THREADS;i++){
      workers[i].join();
	}
#endif
	
	//Print result
   for(int i=0; i<IMG_SIZE; i++){	
		printf("index:%d pixel:%d\n",i,img[i]); 
	}
	return 0;
}

