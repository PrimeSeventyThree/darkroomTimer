/*
 * File: MemoryUtils.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Monday, 17th February 2025 9:22:34 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Monday, 17th February 2025 11:21:34 pm
 * Modified By: Andrei Grichine (andrei.grichine@gmail.com>)
 * -----
 * Copyright 2025 - 2025, Prime73 Inc. All Rights Reserved.
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential.
 * -----
 * HISTORY:
 */

/**
 * @brief Returns the number of bytes currently free in RAM.
 *
 * @return the number of free bytes in RAM
 */
 int freeRam()
 {
   extern char __bss_end;
   extern char *__brkval;
 
   int free_memory;
   if (reinterpret_cast<int>(__brkval) == 0)
   {
     // If heap is not being used, free memory is the space between the end of the static data
     // and the start of the stack
     free_memory = reinterpret_cast<int>(&free_memory) - reinterpret_cast<int>(&__bss_end);
   }
   else
   {
     // If heap is being used, free memory is the space between the end of the heap and the start of the stack
     free_memory = reinterpret_cast<int>(&free_memory) - reinterpret_cast<int>(__brkval);
   }
 
   return free_memory;
 }
 
 