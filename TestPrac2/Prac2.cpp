//==============================================================================
// Copyright (C) John-Philip Taylor
// tyljoh010@myuct.ac.za
//
// This file is part of the EEE4084F Course
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//==============================================================================
#include <algorithm>
#include "Prac2.h"
//------------------------------------------------------------------------------



// This is each thread's "main" function.  It receives a unique ID
void* Thread_Main(void* Parameter){
 int ID = *((int*)Parameter);

 pthread_mutex_lock(&Mutex);
  printf("Hello from thread %d\n", ID);
 pthread_mutex_unlock(&Mutex);

 for(int j = 0; j < 1e6; j++); // Do some heavy calculation

 pthread_mutex_lock(&Mutex);
  printf("Thread %d: I QUIT!\n", ID);
 pthread_mutex_unlock(&Mutex);

 return 0;
}

const std::vector<unsigned int> Fill_Buffer(int y, int x) {
	std::vector<unsigned int> pixels;
	// std::cout << "y: " << y <<" x: " << x << std::endl;
	for (int row = y-4; row <= y+4; ++row)
	{
		// std::cout << "row: " << row << std::endl;
		//zero fill the pixels outside the image
		if(row < 0 || row >= Input.Height) {
			// std::cout << "row: " << row << std::endl;
			for (int col = 0; col < 9; ++col){
				// std::cout << "zero row" << std::endl;
				pixels.push_back((unsigned int)0);
			}
			continue;
		}
		// std::cout << "non-zero row" << std::endl;
		for (int col = x - 4*Input.Components; col <= x + 4*Input.Components; col += Input.Components)
		{
			//zero fill the pixels outside the image
			if(col < 0 || col >= Input.Width*Input.Components) {
				// std::cout << "zero col" << std::endl;
				pixels.push_back((unsigned int)0);
				continue;
			}
			// std::cout << "non-zero col" << std::endl;
			pixels.push_back((unsigned int)Input.Rows[row][col]);
		}
	}
	return pixels;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv){
	using namespace std;
	int j;
	 // Initialise everything that requires initialisation
	tic();
	pthread_mutex_init(&Mutex, 0);

	 // Read the input image
	if(!Input.Read("Data/greatwall.jpg")){
	  	printf("Cannot read image\n");
	  	return -1;
	}

	 // Allocated RAM for the output image
	if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return -2;
	if(!Buffer.Allocate(9, 9, Input.Components)) return -2;

	 // This is example code of how to copy image files ----------------------------
	printf("Start of sequential solution.... \n");
	for(j = 0; j < 10; j++){
	 	tic();
		int x, y;
	  	for(y = 0; y < Input.Height; y++){
	   		for(x = 0; x < Input.Width*Input.Components; x++){	   	
	   			std::vector<unsigned int> pixels = Fill_Buffer(y,x);
	   			// Print_Pixels(pixels);
				unsigned int median = Get_Median(pixels);
				// sort(pixels.begin(), pixels.end());
				// Print_Pixels(pixels);
				// cout <<"Median: "<< median  <<endl<<endl;	
	    		Output.Rows[y][x] = (unsigned char)median;
	   		}
	  	}
	  	printf("Time = %lg ms\n", (double)toc()/1e-3);
	} 
	printf("End of sequential solution...\n\n");
	 // End of example -------------------------------------------------------------

	//comment out parallel part for now
	/*
	 // Spawn threads...
	 int       Thread_ID[Thread_Count]; // Structure to keep the tread ID
	 pthread_t Thread   [Thread_Count]; // pThreads structure for thread admin

	 for(j = 0; j < Thread_Count; j++){
	  Thread_ID[j] = j;
	  pthread_create(Thread+j, 0, Thread_Main, Thread_ID+j);
	 }

	 // Printing stuff is a critical section...
	 pthread_mutex_lock(&Mutex);
	  printf("Threads created :-)\n");
	 pthread_mutex_unlock(&Mutex);

	 tic();
	 // Wait for threads to finish
	 for(j = 0; j < Thread_Count; j++){
	  if(pthread_join(Thread[j], 0)){
	   pthread_mutex_lock(&Mutex);
	    printf("Problem joining thread %d\n", j);
	   pthread_mutex_unlock(&Mutex);
	  }
	 }

	 // No more active threads, so no more critical sections required
	 printf("All threads have quit\n");
	 printf("Time taken for threads to run = %lg ms\n", toc()/1e-3);
	 */

	 // Write the output image
	 if(!Output.Write("Data/Output.jpg")){
	  printf("Cannot write image\n");
	  return -3;
	 }

	 // Clean-up
	 pthread_mutex_destroy(&Mutex);
	 return 0;
}
//------------------------------------------------------------------------------
