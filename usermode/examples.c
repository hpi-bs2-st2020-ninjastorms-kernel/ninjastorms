
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

void task_a(void)
{
  printf("a: My pid: %i\n", get_pid());
  print_tasks_info();
  unsigned int n = 0;

  while (1)
  {
    printf("  task a: %i\n", n++);
    volatile int i;
    sleep(10000000);
    if (n == 7)
    {
      print_tasks_info();
    }
  }
}

void task_b(void)
{
  printf("b: My pid: %i\n", get_pid());
  unsigned int n = 0;

  while (1)
  {
    printf("  task b: %i\n", n++);
    volatile int i;
    sleep(10000000);
    if (n > 7)
    {
      //Enough of b!
      return;
      //This will automatically call exit()
    }
  }
}

void task_c(void)
{
  int my_pid = get_pid();
  printf("c: My pid: %i, my parent is pid %i\n", my_pid, get_parent_pid());
  unsigned int n = 0;

  while (1)
  {
    printf("  task c: %i\n", n++);
    if (n > 3)
    {
      //That's enough for me
      return;
    }
    volatile int i;
    sleep(10000000);
  }
}

void task_d(void)
{
  int my_pid = get_pid();
  printf("d: My pid: %i\n", my_pid);
  create_process(&task_c);
  unsigned int n = 0;
  /* Testing for permissions
  int direct_call_result = add_task(&task_c);
  if(direct_call_result<0){
    printf("add task failed with errno %i \n",errno);
  }*/
  while (1)
  {
    printf("  task d: %i\n", n++);
    volatile int i;
    sleep(10000000);
    if (n > 25)
    {
      //That's enough for everyone!
      print_tasks_info();
      shutdown();
    }
  }
}

void task_e(void)
{
  unsigned int n = 0;
  printf("e: My pid: %i\n", get_pid());
  while (1)
  {
    printf("  task e: %i\n", n++);
    volatile int i;
    sleep(10000000);
    if (n == 10)
    {
      create_process(&task_a);
    }
  }
}

void task_receiver(void)
{
  int c = 0;
  while (c < 8)
  {
    volatile int i;
    sleep(20000000);
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

void task_sender(void)
{
  int receiver = create_process(&task_receiver);
  unsigned int c = 0;
  char message[] = "Shinobi";
  while (c < 8)
  {
    volatile int i;
    sleep(10000000);
    errno = 0;
    if (ipc_buffer_send(message[c], receiver) == -1)
    {
      if (errno == EBUFFERCLOSED)
      {
        printf("Receiver not ready \n");
      }
      else if (errno == EBUFFERFULL)
      {
        printf("Buffer full, not sending \n");
      }
    }
    else
    {
      //printf("Sent: %i \n", message[c]);
      c++;
    }
  }
}

int task_calculate(void)
{
  int n1, n2, i, gcd, lcm;
  n1 = 7;
  n2 = 24;
  //for(int j=0;j<150000000; ++j);
  printf("calculating lcm of %i and %i \n", n1, n2);
  for (i = 1; i <= n1 && i <= n2; ++i)
  {
    if (n1 % i == 0 && n2 % i == 0)
      gcd = i;
  }
  lcm = (n1 * n2) / gcd;
  //for(int j=0;j<150000000; ++j);
  return lcm;
}

void task_wait(void)
{
  //pid_t calc_task = create_process(&task_calculate);
  printf("Starting wait\n");
  float result = wait_on_pid(3);
  printf("lcm is %i\n", (int)result);
  kill(3); // Kill process to make sure the zombie is deleted
}

int task_exit_test(void)
{
  return get_pid();
}

int task_pass_a(void)
{
  int n = 0;
  while (1)
  {
    sleep(5000000);
    printf("A: %i\n", n++);
    if (n % 5 == 0)
    {
      printf("Passing!\n");
      pass();
    }
  }
}

int task_pass_b(void)
{
  int n = 0;
  while (1)
  {
    sleep(5000000);
    printf("B: %i\n", n++);
    if (n % 5 == 0)
    {
      printf("Passing!\n");
      pass();
    }
  }
}

void task_print_info_periodically(void)
{
  while (1)
  {
    sleep(500000000);
    print_tasks_info();
  }
}

void task_recursive_exit(void)
{
  create_process(&task_recursive_exit);
  print_tasks_info();
  sleep(500000);
  return;
}

unsigned int shared_mem = unlocked;
int count_value = 0;

void task_mutex_a(void)
{
  for (int i = 0; i < 10; i++)
  {
    lock_mutex(&shared_mem);
    int tmp_value = count_value;
    sleep(5000000);
    tmp_value++;
    count_value = tmp_value;
    printf("A: %i\n", count_value);
    unlock_mutex(&shared_mem);
    sleep(5000000);
  }
}

void task_mutex_b(void)
{
  for (int i = 0; i < 10; i++)
  {
    lock_mutex(&shared_mem);
    int tmp_value = count_value;
    sleep(5000000);
    tmp_value--;
    count_value = tmp_value;
    printf("B: %i\n", count_value);
    unlock_mutex(&shared_mem);
    sleep(5000000);
  }
}

int task_receive_and_calculate(void)
{
  ipc_buffer_open(2);
  int a = read_ipc_buffer_and_block();
  int b = read_ipc_buffer_and_block();
  printf("Read %i and %i as input from IPC\n", a, b);
  int result = ackermann(a, b);
  return result;
}

unsigned int mutex = unlocked;
int printing_queue[] = {4};
int queue_start = 0;
int queue_end = 0;

int write_queue(int value)
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

int read_queue(void)
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

void task_queue_printer(void)
{
  while (1)
  {
    //Read from printing queue
    lock_mutex(&mutex);
    int read_value = read_queue();
    unlock_mutex(&mutex);
    if (read_value != -1)
    {
      printf("READING RESULT: %i\n", read_value);
    }
    else
    {
      pass();
    }
  }
}

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
    send_when_ready(&message, 2, calculator);
    // Wait on results
    int result = wait_on_pid(calculator);

    // Write into printing queue
    int queue_result = -1;
    while (queue_result == -1)
    {
      printf("Trying to write %i \n", result);
      lock_mutex(&mutex);
      queue_result = write_queue(result);
      unlock_mutex(&mutex);
      pass();
    }
  }
  kill(printer);
}
