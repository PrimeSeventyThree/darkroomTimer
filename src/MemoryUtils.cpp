/*
 * File: MemoryUtils.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Monday, 17th February 2025 9:22:34 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Monday, 17th February 2025 11:59:42 pm
 * Modified By: Andrei Grichine (andrei.grichine@gmail.com>)
 * -----
 * Copyright 2019 - 2025, Prime73 Inc. MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
 
 