#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdint.h>
#include <pthread.h>
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex> 
using namespace std;


#define IMG2_SIZE 1000000

#define TASK 3
#define NUM_THREADS 8


std::mutex mtx;


//  Task5.2:
void calc_hist(unsigned char* img, long start, long stop, unsigned char *hist, long thread){
	for (long i=start;i<stop;i++){
		hist[(255*thread)+img[i]] += 1;
	}
}


// Task5.3:
int work_id_queue[] = {0,1,2,3,4,5,6,7,8,9,10};
std::atomic<int> last_used_index; //

void calc_histogram_t3(unsigned char* img, long start, long stop, unsigned char *hist){
	int my_id = work_id_queue[++last_used_index];
	printf("my_id:%d\n",my_id); 
	for(long x=0;x<1000000;x++){}
	
	for (long i=start;i<stop;i++){
		hist[(255*(my_id-1))+img[i]] += 1;
	}
}

//  Task5.4:
void bar(int *A, int *B, int start, int N, int thrcount, int *result) {
	int th_result = 0;
	for (int i = start; i < N; i+=(1+thrcount)) { 
		th_result += A[i] * B[i];
	}
	*result += th_result;
}



int main (){
	unsigned char img2[IMG2_SIZE+1];
	unsigned char hist[255*NUM_THREADS]; // shared array between threads
	unsigned char finalhist[255]; //Final histogram to show
	unsigned char hist_test[255];
	
	unsigned char hists[NUM_THREADS][255];
	printf("Generate random image\n"); 
	for (int i =0;i<IMG2_SIZE;i++){ //Init image with random numbers
		img2[i] = rand() % 255;
	}

	for(int i=0; i<255; i++){ //Initialize test and final histogram to zero
		hist_test[i] = 0;
		finalhist[i] = 0;
	}
	
	for(int i=0; i<255*NUM_THREADS; i++){	//Init shared vector
		hist[i] = 0;
	}
	
#if TASK==2
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
	
	
	for(long i =0;i<NUM_THREADS;i++){ //Combine results to single histogram:
		for(long j =0;j<255;j++){
			finalhist[j] += hist[i*255+j];
		}
	}
	
	calc_hist(img2, 0, IMG2_SIZE, hist_test,0); //Calculate reference by using main thread
	
	int wrongcount = 0;
	for(int i =0;i<255;i++){
		if (hist_test[i] != finalhist[i]){
			printf("hist_test[i]:%d  hist[i]:%d\n",hist_test[i],finalhist[i]);
			wrongcount++;
		}
	}
	printf("wrongcount:%d\n",wrongcount);


#elif TASK==3
	printf("Task5.3 Starts\n"); 
	
	std::thread threads[NUM_THREADS];
	std::vector<std::thread> workers;
	for (long i=0;i<NUM_THREADS;i++){
		long start = IMG2_SIZE/NUM_THREADS*i;
		long stop = IMG2_SIZE/NUM_THREADS*(i+1);
		printf("Thread:%ld start:%ld stop:%ld\n",i,start,stop); 
		workers.push_back(std::thread(calc_histogram_t3, img2, start, stop, hist));
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
	
	calc_hist(img2, 0, IMG2_SIZE, hist_test,0); //Calculate reference by using main thread

	int wrongcount = 0;
	for(int i =0;i<255;i++){
		if (hist_test[i] != finalhist[i]){
			printf("hist_test[i]:%d  hist[i]:%d\n",hist_test[i], finalhist[i]);
			wrongcount++;
		}
	}
	printf("wrongcount:%d\n",wrongcount);
	
	
	
#elif TASK==4
	#define THREAD_COUNT4 2
	printf("Task5.4 Starts\n"); 
	int arr1[] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
	int arr2[] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};

	printf("Task5.4 Starts\n"); 
	std::thread threads[2];
	std::vector<std::thread> workers;		
	
	int N = 16;
	int calcresult = 0;
	for (long i=0;i<THREAD_COUNT4;i++){
		printf("Starting thread:%ld\n",i); //
		workers.push_back(std::thread(bar, arr1, arr2, i, N, THREAD_COUNT4, &calcresult));
	}

	for (int i=0;i<THREAD_COUNT4;i++){
	    workers[i].join();
	}
	printf("threads finished\n");

	printf("result:%d\n",calcresult);

#endif

	return 0;
}

