
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

#include "scheduler.h"
#include "tasks.h"
#include "kernel/memory.h"
#include "kernel/drivers/timer.h"
#include "kernel/interrupt.h"
#include "kernel/interrupt_handler.h"

#include <stdio.h>

#if BOARD_VERSATILEPB
#  define TIMER_LOAD_VALUE 0x2000
#endif

#if BOARD_EV3
#  define TIMER_LOAD_VALUE 0x10000
#endif

int buffer_start = 0;
int buffer_end   = 0;
int isRunning    = 0;
task_t* ring_buffer[MAX_TASK_NUMBER] = { 0 };

// TODO: disable interrupts during insertion
void
ring_buffer_insert (task_t *task)
{
  int new_end = (buffer_end + 1) % MAX_TASK_NUMBER;
  if (new_end != buffer_start)
    {
      ring_buffer[buffer_end] = task;
      buffer_end = new_end;
    }
}

task_t*
ring_buffer_remove (void)
{
  if (buffer_start == buffer_end)
    return &tasks[0];

  task_t* task = ring_buffer[buffer_start];
  buffer_start = (buffer_start + 1) % MAX_TASK_NUMBER;
  return task;
}


void
set_current_task_update_state()
{
  int32_t initial_buffer_start = buffer_start;
  
  do{
    current_task = ring_buffer_remove();
    if(current_task->state == TASK_RUNNING){
      return;
    }
    if(current_task->state == TASK_WAITING){
      int8_t wait_done = update_wait();
      if(wait_done){
        return;
      } //Wait not done, select another process
    }
    if(current_task->state == TASK_DONE){
      //TODO clear done task
    }
  } while(buffer_start != initial_buffer_start);
  printf("No task not marked done or waiting!\n");
}

void
schedule_after_exit(void)
{
    set_current_task_update_state();
    printf("New task will be Task %i",current_task->pid);
    restore_errno();
    load_current_task_state();
}

void
schedule_after_wait (void)
{
  store_errno();
  ring_buffer_insert(current_task);
  set_current_task_update_state();
  restore_errno();
  load_current_task_state();
}

void
schedule (void)
{
  store_errno();
  ring_buffer_insert(current_task);
  set_current_task_update_state();
  restore_errno();
}


void
start_scheduler (void)
{
  if (!isRunning)
    {
      current_task = ring_buffer_remove();
      isRunning = 1;
      timer_stop();
      init_interrupt_handling();
      timer_start(TIMER_LOAD_VALUE);
      load_current_task_state();
    }
}


void
rebuild_ring_buffer(void)
{
    int buffer_position = 0;
    buffer_start = 0;
    for(int i=0;i<MAX_TASK_NUMBER;i++){
        if(tasks[i].valid == 1){
            ring_buffer[buffer_position] = &tasks[i];
            buffer_position++;
        }
    }
    buffer_end = buffer_position;
}

int
insert_task(task_t* new_task)
{
    ring_buffer_insert(new_task);
}



void
print_ring_buffer_debug_info (void)
{
    printf("------------\n----Scheduling-Buffer----\n");
    printf("Buffer start: %i, Buffer end: %i\n",buffer_start,buffer_end);
    int tasksbufferinfo[MAX_TASK_NUMBER] = {0};
    for(int i=0;i<MAX_TASK_NUMBER;i++){
        if(ring_buffer[i]!=0){
            tasksbufferinfo[i]=ring_buffer[i]->pid;
        }
    }
    printf("[%i][%i][%i][%i][%i][%i][%i][%i]\n",
           tasksbufferinfo[0],tasksbufferinfo[1],tasksbufferinfo[2],tasksbufferinfo[3],
           tasksbufferinfo[4],tasksbufferinfo[5],tasksbufferinfo[6],tasksbufferinfo[7]);
    printf("[%i][%i][%i][%i][%i][%i][%i][%i]\n",
           tasksbufferinfo[8],tasksbufferinfo[9],tasksbufferinfo[10],tasksbufferinfo[11],
           tasksbufferinfo[12],tasksbufferinfo[13],tasksbufferinfo[14],tasksbufferinfo[15]);
}
