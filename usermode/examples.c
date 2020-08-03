
/******************************************************************************
 *       ninjastorms - shuriken operating system                              *
 *                                                                            *
 *    Copyright (C) 2013 - 2016  Andreas Grapentin et al.                     *
 *                                                                            *
 *    This program is free software: you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation, either version 3 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ******************************************************************************/

#include "examples.h"
#include "usermode/utilities.h"

#include <syscall.h>
#include <stdio.h>
#include <errno.h>
#include <thread.h>
#include <sys/types.h>
#include <math.h>

// Print data received via ipc.
void task_receiver(void)
{
  int c = 0;
  while (c < 8)
  {
    volatile int i;
    delay(20000000);
    int validity = 0;
    int received = read_ipc_buffer_and_check(&validity);
    if (!validity)
    {
      printf("Oops forgot to open buffer\n");
      ipc_buffer_open(16);
      continue;
    }
    printf("Received: %c\n", received);
    c++;
  }
  printf("Closing buffer!\n");
  ipc_buffer_close();
  exit(0);
}

// Create a new receiver task and send data to it.
void task_sender(void)
{
  int receiver = create_process(&task_receiver);
  unsigned int c = 0;
  int message[8] = {83, 104, 105, 110, 111, 98, 105, 0};
  send_when_ready(message, 8, receiver);
}

// Print task info every couple of seconds.
void task_print_info_periodically(void)
{
  while (1)
  {
    delay(500000000);
    print_tasks_info();
  }
}

// Create a new task with same function and exit.
void task_recursive_exit(void)
{
  create_process(&task_recursive_exit);
  print_tasks_info();
  delay(500000);
  return;
}

// With the 2 tasks below you can see the implementation
// of mutex in action. Count_value will be 0 at the end.
// Try commenting out the lines about mutex and see what
// the result is otherwise.

unsigned int shared_mem = unlocked;
int count_value = 0;

void task_mutex_a(void)
{
  for (int i = 0; i < 10; i++)
  {
    lock_mutex(&shared_mem);
    int tmp_value = count_value;
    delay(5000000);
    tmp_value++;
    count_value = tmp_value;
    printf("A: %i\n", count_value);
    unlock_mutex(&shared_mem);
    delay(5000000);
  }
  printf("A done!\n");
}

void task_mutex_b(void)
{
  for (int i = 0; i < 10; i++)
  {
    lock_mutex(&shared_mem);
    int tmp_value = count_value;
    delay(5000000);
    tmp_value--;
    count_value = tmp_value;
    printf("B: %i\n", count_value);
    unlock_mutex(&shared_mem);
    delay(5000000);
  }
  printf("B done!\n");
}

// Receive two numbers via ipc and calculate the value of the ackermann
// function for them.
int task_receive_and_calculate(void)
{
  ipc_buffer_open(2);
  int a = read_ipc_buffer_and_block();
  int b = read_ipc_buffer_and_block();
  printf("Read %i and %i as input from IPC\n", a, b);
  int result = ackermann(a, b);
  return result;
}

// A queue.
unsigned int mutex = unlocked;
int printing_queue[4];
int queue_start = 0;
int queue_end = 0;

int enqueue(int value)
{
  int new_end = (queue_end + 1) % 4;
  if (new_end != queue_start)
  {
    printing_queue[queue_end] = value;
    queue_end = new_end;
    return 0;
  }
  errno = EBUFFERFULL;
  return -1;
}

int dequeue(void)
{
  if (queue_start == queue_end)
  {
    errno = EBUFFEREMPTY;
    return -1;
  }
  int value = printing_queue[queue_start];
  queue_start = (queue_start + 1) % 4;
  return value;
}


// Print values from the queue.
void task_queue_printer(void)
{
  while (1)
  {
    delay(100000000);
    //Read from printing queue
    lock_mutex(&mutex);
    int read_value = dequeue();
    unlock_mutex(&mutex);
    if (read_value != -1)
    {
      printf("READING RESULT: %i\n", read_value);
    }
    else
    {
      yield();
    }
  }
}

// In this example, you can see 3 different types of 
// communication between processes: wait/return, ipc-buffer
// and shared memory with mutes.
// It also demonstrates a multitude of syscalls.

void task_orchestrate_example(void)
{
  pid_t printer = create_process(&task_queue_printer);
  int value_a = 3;
  for (int m = 1; m < 11; m++)
  {
    // Start calculater
    pid_t calculator = create_process(&task_receive_and_calculate);

    // Send values to calculater
    int message[] = {value_a, m};
    send_when_ready(message, 2, calculator);
    // Wait on results
    int result = wait_on_pid(calculator);

    // Write into printing queue
    int queue_result = -1;
    while (queue_result == -1)
    {
      printf("Trying to write %i \n", result);
      //lock_mutex(&mutex);
      queue_result = enqueue(result);
      //unlock_mutex(&mutex);
      yield();
    }
  }
  delay(300000000);
  kill(printer);
}


// task_cooperative_1 and 2 demonstrate cooperative multitasking. With
// cooperative multitasking turned on (in kernel/main.c), Player 1 should never
// print its third line, if they get scheduled in order.
void
task_cooperative_1(void)
{
    printf("Player 1: Yield.\n");
    yield();
    printf("Player 1: I will again yield. But please let me act again.\n");
    yield();
    printf("Now I am happy :)\n");
}

void
task_cooperative_2(void)
{
    printf("Player 2: Another task may run again.\n");
    yield();
    printf("Player 2: I am the bad guy.\n");
    while(1);
}

