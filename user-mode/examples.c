
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

#include <syscall.h>

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include "examples.h"

int read_ipc_buffer_and_block(void)
{
    int DELAY = 50000;
    int result = 0;
    do{
        result = ipc_buffer_read();
        if (result == -1){
            for(int i=0; i<DELAY; ++i);
        }
    } while(result == -1 && errno == EBUFFEREMPTY);
    return result;
}

int32_t read_ipc_buffer_and_check(int32_t* validity)
{
    *validity = 1;
    int32_t result = ipc_buffer_read();
    if(result == -1 && errno != 0){
        // The result is not literally "-1", but indicates an error occured (errno is set to 0 by ipc_buffer_read())
        // thus the return value is not valid
        *validity = 0;
    }
    return result;
}

void
task_a(void)
{
  printf("a: My pid: %i\n", get_pid());
  print_tasks_info();
  unsigned int n = 0;

  while (1)
  {
    printf("  task a: %i\n", n++);
    volatile int i;
    for (i = 0; i < 10000000; ++i);
    if (n == 7)
    {
      print_tasks_info();
    }
  }
}

void
task_b(void)
{
  printf("b: My pid: %i\n", get_pid());
  unsigned int n = 0;

  while (1)
  {
    printf("  task b: %i\n", n++);
    volatile int i;
    for (i = 0; i < 10000000; ++i);
    if (n > 7)
    {
      //Enough of b!
      return;
      //This will automatically call exit()
    }
  }
}

void
task_c(void)
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
    for (i = 0; i < 10000000; ++i);
  }
}

void
task_d(void)
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
    for (i = 0; i < 10000000; ++i);
    if (n > 25)
    {
      //That's enough for everyone!
      print_tasks_info();
      shutdown();
    }
  }
}

void
task_e(void)
{
  unsigned int n = 0;
  printf("e: My pid: %i\n", get_pid());
  while (1)
  {
    printf("  task e: %i\n", n++);
    volatile int i;
    for (i = 0; i < 10000000; ++i);
    if (n == 10)
    {
      create_process(&task_a);
    }
  }
}

void
task_receiver(void)
{
  int c = 0;
  while (c < 8)
  {
    volatile int i;
    for (i = 0; i < 20000000; ++i);
    int validity = 0;
    int received = read_ipc_buffer_and_check(&validity);
    if (!validity)
    {
      printf("Oops forgot to open buffer\n");
      ipc_buffer_open(16);
    }
    printf(" Received: %c\n", received);
    c++;
  }
  printf("Closing buffer!\n");
  ipc_buffer_close();
  exit(0);
}

void
task_sender(void)
{
  int receiver = create_process(&task_receiver);
  unsigned int c = 0;
  char message[] = "Shinobi";
  while (c < 8)
  {
    volatile int i;
    for (i = 0; i < 10000000; ++i);
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

int
task_calculate(void)
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

void
task_wait(void)
{
  //pid_t calc_task = create_process(&task_calculate);
  printf("Starting wait\n");
  float result = wait_on_pid(3);
  printf("lcm is %i\n", (int)result);
}

int
task_exit_test(void)
{
  return get_pid();
}

int
task_pass_a(void)
{
  int n = 0;
  while(1)
  {
    for(int j=0;j<5000000; ++j);
    printf("A: %i\n", n++);
    if(n%5 == 0){
      printf("Passing!\n");
      pass();
    }
  }
}

int
task_pass_b(void)
{
  int n = 0;
  while(1)
  {
    for(int j=0;j<5000000; ++j);
    printf("B: %i\n", n++);
    if(n%5 == 0){
      printf("Passing!\n");
      pass();
    }
  }
}