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

	int x, y;
	// figure out which part (block) of the picture to process
	int ID = *((int*)Parameter);
	// int i = ID/sqrt(Thread_Count);
	// int x_start = (ID - i*sqrt(Thread_Count))*Thread_Col_size;
	// int y_start = i*Thread_Row_size;
	// int x_end = x_start + Thread_Col_size;
	// int y_end = y_start + Thread_Row_size;

	//adjecent rows
	Thread_Row_size = Input.Height/Thread_Count;
	if (Input.Height%Thread_Count != 0){Thread_Row_size = Input.Height/Thread_Count+1;}
	// int i = ID/Thread_Count;
	int x_start = 0;
	int y_start = ID*Thread_Row_size;
	int x_end = Input.Width*Input.Components;
	int y_end = y_start + Thread_Row_size;

	pthread_mutex_lock(&Mutex);
  	printf("Hello from thread %d ---y:%d  y1:%d x:%d  x1:%d\n", ID,y_start,y_end,x_start,x_end);
	 pthread_mutex_unlock(&Mutex);

  	for(y = y_start; y < y_end; y++){
  		if(y >= Input.Height){break;}
   		for(x = x_start; x < x_end; x++){	 
   			if(x > Input.Width*Input.Components){break;}  	
   			std::vector<unsigned int> pixels = Fill_Buffer(y,x);
			unsigned int median = Get_Median(pixels);
			pthread_mutex_lock(&Mutex);	
    		Output.Rows[y][x] = (unsigned char)median;
    		pthread_mutex_unlock(&Mutex);
   		}
  	}


 pthread_mutex_lock(&Mutex);
  printf("Thread %d: I QUIT!\n", ID);
 pthread_mutex_unlock(&Mutex);

 return 0;
}

// implementation of the sequential solution (golden measurement)
void Sequentional(void){
	int x, y;
		  	for(y = 0; y < Input.Height; y++){
		   		for(x = 0; x < Input.Width*Input.Components; x++){	
		   			std::vector<unsigned int> pixels = Fill_Buffer(y,x);
					unsigned int median = Get_Median_Bubble(pixels);
					// Print_Pixels(pixels);
					// std::cout <<"Median" <<median<<std::endl;
						
		    		Output.Rows[y][x] = (unsigned char)median;
		   		}
		  	}
}

const std::vector<unsigned int> Fill_Buffer(int y, int x) {
	std::vector<unsigned int> pixels;
	for (int row = y-4; row <= y+4; row++)
	{
		//zero fill the pixels outside the image
		if(row < 0 || row >= Input.Height) {
			for (int col = 0; col < 9; col++){
				pixels.push_back((unsigned int)0);
			}
			continue;
		}
		// std::cout << "non-zero row" << std::endl;
		for (int col = x - 4*Input.Components; col <= x + 4*Input.Components; col += Input.Components)
		{
			//zero fill the pixels outside the image
			if(col < 0 || col >= Input.Width*Input.Components) {
				pixels.push_back((unsigned int)0);
				continue;
			}
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
	std::ostringstream oss;
	pthread_mutex_init(&Mutex, 0);

	 // Read the input image
	if(!Input.Read("Data/small.jpg")){
	  	printf("Cannot read image\n");
	  	return -1;
	}

	 // Allocated RAM for the output image
	if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return -2;
	if(!Buffer.Allocate(9, 9, Input.Components)) return -2;
	//comment out serial part for now
	 // This is example code of how to copy image files ----------------------------
	 oss<<"Start of sequential solution.... \n";
	
	for(j = 0; j < 4; j++){
	 	tic();
		Sequentional();
		oss << "Time =  "<<toc()/1e-3<<" ms\n";

	  	printf("Time = %lg ms\n", (double)toc()/1e-3);
	} 
	printf("End of sequential solution...\n\n");
	oss<<"End of sequential solution...\n\n";
	 // End of example -------------------------------------------------------------

	
	 // Spawn threads...
	
 	Thread_Row_size = Input.Height/sqrt(Thread_Count) + 1;
 	Thread_Col_size = Input.Width*Input.Components/sqrt(Thread_Count) + Input.Components;
	
	 oss<<"Start of parallel solution...\n";
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
	 oss << "Time taken for threads to run =  "<<toc()/1e-3<<" ms\n";


	 // Write the output image
	 if(!Output.Write("output.jpg")){
	  printf("Cannot write image\n");
	  return -3;
	 }
	 
	 // record results
	ofstream myfile;
	myfile.open ("results.txt");
	myfile << oss.str();
	myfile.close();
	
	 // Clean-up
	 pthread_mutex_destroy(&Mutex);
	 return 0;
}
//------------------------------------------------------------------------------
