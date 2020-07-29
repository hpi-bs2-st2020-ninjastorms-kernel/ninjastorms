
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

#include "main.h"

#include "kernel/drivers/button.h"
#include "kernel/scheduler.h"
#include "kernel/tasks.h"
#include "memory.h"
#include "kernel/utilities.h"

#include "syscall.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

static void
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

static void
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

static void
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

static void
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

static void
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

static void
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

static void
task_sender(void)
{
  int receiver = create_process(&task_receiver);
  unsigned int c = 0;
  char message[] = "Shinobi";
  while (c < 8)
  {
    volatile int i;
    for (i = 0; i < 10000000; ++i)
      ;
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

static int
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

static void
task_wait(void)
{
  //pid_t calc_task = create_process(&task_calculate);
  printf("Starting wait\n");
  float result = wait_on_pid(3);
  printf("lcm is %i\n", (int)result);
}

static int
task_exit(void)
{
  return get_pid();
}

static void
user_mode_init(void)
{
  printf("User mode initialized with pid: %i\n", get_pid());
  /*pid_t e_pid = create_process(&task_e);
    create_process(&task_b);
    create_process(&task_d);
    create_process(&task_sender);
    print_tasks_info();
    for(int i=0;i<150000000; ++i);
    kill(e_pid); */

  create_process(&task_wait);
  create_process(&task_calculate);

  for (int i = 0; i < 150000000; ++i);
  print_tasks_info();
  while (1); //init will run forever
}

char shuriken[] =
    "                 /\\\n"
    "                /  \\\n"
    "                |  |\n"
    "              __/()\\__\n"
    "             /   /\\   \\\n"
    "            /___/  \\___\\\n";

int kernel_main(void)
{
  puts("This is ninjastorms OS");
  puts("  shuriken ready");
  puts(shuriken);

  add_task(&user_mode_init);
  start_scheduler();

  puts("All done. ninjastorms out!");

  return 0;
}
