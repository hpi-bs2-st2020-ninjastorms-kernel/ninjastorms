
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
#include "user.h"

#include "syscall.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

static void
task_a (void)
{
  printf("a: My pid: %i\n",get_pid());
  print_tasks_info();
  unsigned int n = 0;

  while (1)
    {
        printf("  task a: %i\n", n++);
        volatile int i;
        for (i = 0; i < 10000000; ++i);
        if(n==7){
            print_tasks_info();
        }
    }
}

static void
task_b (void)
{
  printf("b: My pid: %i\n",get_pid());
  unsigned int n = 0;

  while (1)
    {
      printf("  task b: %i\n", n++);
      volatile int i;
      for (i = 0; i < 10000000; ++i);
      if(n>7){
            //Enough of b!
          return;
          //This will automatically call exit()
      }
    }
}

static void
task_c (void)
{
  int my_pid = get_pid();
  printf("c: My pid: %i, my parent is pid %i\n",my_pid, get_parent_pid());
  unsigned int n = 0;

  while (1)
    {
      printf("  task c: %i\n", n++);
      if(n>3){
          //That's enough for me
          exit();
      }
      volatile int i;
      for (i = 0; i < 10000000; ++i);
    }
}

static void
task_d (void)
{
  int my_pid = get_pid();
  printf("d: My pid: %i\n",my_pid);
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
        if(n>25){
            //That's enough for everyone!
            print_tasks_info();
            // Send shutdown signal to init
            ipc_buffer_send(99,1);
        }
    }
}

static void
task_e (void)
{
  unsigned int n = 0;
  printf("e: My pid: %i\n",get_pid());
  while (1)
    {
      printf("  task e: %i\n", n++);
      volatile int i;
      for (i = 0; i < 10000000; ++i);
      if(n==10){
          create_process(&task_a);
      }
    }
}

static void
task_receiver (void)
{
    int c = 0;
    while(c<8){
        volatile int i;
        for (i = 0; i < 20000000; ++i);
        int validity = 0;
        int received = read_ipc_buffer_and_check(&validity);
        if(!validity){
          printf("Oops forgot to open buffer\n");
          ipc_buffer_open(16);
        }
        printf(" Received: %c\n", received);
        c++;
    }
    printf("Closing buffer!\n");
    ipc_buffer_close();
    exit();
}

static void
task_sender (void)
{
    int receiver = create_process(&task_receiver);
    unsigned int c = 0;
    char message[] = "Shinobi";
    while (c<8)
    {
        volatile int i;
        for (i = 0; i < 10000000; ++i);
        errno = 0;
        if (ipc_buffer_send(message[c],receiver) == -1){
          if(errno == EBUFFERCLOSED){
            printf("Receiver not ready \n");
          } else if(errno == EBUFFERFULL){
            printf("Buffer full, not sending \n");
          }
        } else{
            //printf("Sent: %i \n", message[c]);
            c++;
        }
    }
}


static void
user_mode_init(void)
{
    printf("User mode initialized with pid: %i\n", get_pid());
    // Make sure that processes are not started as root
    pid_t e_pid = create_process_with_uid(&task_e,1);
    create_process_with_uid(&task_b,1);
    create_process_with_uid(&task_d, 1);
    create_process_with_uid(&task_sender,1);
    print_tasks_info();
    for(int i=0;i<150000000; ++i);
    kill(e_pid);
    print_tasks_info();
    ipc_buffer_open(1);
    while(1){
      int result = read_ipc_buffer_and_block();
      /* This is a way to implement some syscall like control on user mode level
      *  with sub processes sending signals to init, to perform tasks only available to root
      */
      if(result == 99){ //shutdown signal sent from subtask
        shutdown();
      }
    } //init will run forever
}


char shuriken[] =
"                 /\\\n"
"                /  \\\n"
"                |  |\n"
"              __/()\\__\n"
"             /   /\\   \\\n"
"            /___/  \\___\\\n";


int
kernel_main (void)
{
  puts("This is ninjastorms OS");
  puts("  shuriken ready");
  puts(shuriken);

  add_task(&user_mode_init);
  start_scheduler();

  puts("All done. ninjastorms out!");

  return 0;
}
