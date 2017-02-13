#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdint.h>
#include <pthread.h>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex> 
using namespace std;


#define IMG2_SIZE 5000000

#define TASK 1
#define NUM_THREADS 8

std::mutex mtx;



void calc_hist(unsigned char* img, long start, long stop, unsigned char *hist, long thread){
	for(int x=0;x<100;x++){
		for (long i=start;i<stop;i++){
			hist[(255*thread)+img[i]] += 1;
		}
	}
}


int main (){
	unsigned char img2[IMG2_SIZE+1];
	unsigned char hist[255*NUM_THREADS]; // shared array between threads
	unsigned char finalhist[255]; //Final histogram to show
	unsigned char hist_test[255];
	
	unsigned char hists[NUM_THREADS][255];
	printf("Gen random image\n"); 
	//Init image with random numbers
	for (int i =0;i<IMG2_SIZE;i++){
		img2[i] = rand() % 255;
	}

	//Init test and final histogram to zero
	for(int i=0; i<255; i++){
		hist_test[i] = 0;
		finalhist[i] = 0;
	}
	
	//Init shared vector
	for(int i=0; i<255*NUM_THREADS; i++){	
		hist[i] = 0;
	}

	printf("Task5.2 Starts\n"); 
	std::thread threads[NUM_THREADS];
	std::vector<std::thread> workers;
	for (long i=0;i<NUM_THREADS;i++){
		long start = IMG2_SIZE/NUM_THREADS*i;
		long stop = IMG2_SIZE/NUM_THREADS*(i+1);
		printf("Thread:%ld start:%ld stop:%ld\n",i,start,stop); 
		workers.push_back(std::thread(calc_hist, img2, start, stop, hist, i));
	}
	
	for (int i=0;i<NUM_THREADS;i++){
       workers[i].join();
	}
	printf("threads finished\n");
	
	
	//Combine results to single histogram:
	for(long i =0;i<NUM_THREADS;i++){
		for(long j =0;j<255;j++){
			finalhist[j] += hist[i*255+j];
		}
	}
	
	
	calc_hist(img2, 0, IMG2_SIZE, hist_test,0); //Calculate reference by using one thread only
	
	
	int wrongcount = 0;
	for(int i =0;i<255;i++){
		if (hist_test[i] != finalhist[i]){
			printf("hist_test[i]:%d  hist[i]:%d\n",hist_test[i],hist[i]);
			wrongcount++;
		}
	}
	printf("wrongcount:%d\n",wrongcount);
	

	return 0;
}

