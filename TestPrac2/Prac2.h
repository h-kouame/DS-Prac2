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

#ifndef Prac2_h
#define Prac2_h
//------------------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
//------------------------------------------------------------------------------

#include "JPEG.h"
#include "Timer.h"
//------------------------------------------------------------------------------

#define Thread_Count 16
//------------------------------------------------------------------------------

// Input and output buffers
JPEG Input;
JPEG Output;
int Buffer_size = 81;
JPEG Buffer;
//------------------------------------------------------------------------------

pthread_mutex_t Mutex; // General-purpose MutEx
//------------------------------------------------------------------------------

int Thread_ID[Thread_Count]; // Structure to keep the tread ID
 pthread_t Thread   [Thread_Count]; // pThreads structure for thread admin
 int Thread_Row_size ;
 int Thread_Col_size;
//------------------------------------------------------------------------------
// The thread "main" function.  Receives a unique thread ID as parameter
void* Thread_Main(void* Parameter);
//------------------------------------------------------------------------------

//helper to fill Buffer with surrounding pixels
const std::vector<unsigned int> Fill_Buffer(int y, int x);

//for debugging
void Print_Pixels(std::vector<unsigned int> pixels) {
	for (int i = 0; i < 9; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
			std::cout << pixels[i*9 + j] << " ";
		}
		std::cout << std::endl;
	}
}

void Sequentional(void);

//get the median of 81 int values
const unsigned int Get_Median(std::vector<unsigned int> pixels) {
	std::sort(pixels.begin(), pixels.end());
	return pixels[Buffer_size/2];
}

#endif
//------------------------------------------------------------------------------
