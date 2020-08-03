
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
#include "kernel/hal.h"
#include "kernel/drivers/timer.h"
#include "kernel/interrupt.h"
#include "kernel/interrupt_handler.h"
#include "kernel/syscall/syscall_dispatcher.h"

#include <stdio.h>
#include <stddef.h>

int buffer_start = 0;
int buffer_end = 0;
int isRunning = 0;
// Ring buffer is used for scheduling.
// All tasks that can be scheduled are placed in the buffer,
// when "schedule()" is called, a valid item is drawn from 
// the start.
task_t *ring_buffer[MAX_TASK_NUMBER] = {0};

int32_t ring_buffer_insert(task_t *task)
{
  int new_end = (buffer_end + 1) % MAX_TASK_NUMBER;
  if (new_end != buffer_start)
  {
    ring_buffer[buffer_end] = task;
    buffer_end = new_end;
    return 0;
  }
  return -1;
}

task_t *
ring_buffer_remove(void)
{
  if (buffer_start == buffer_end)
    return NULL;

  task_t *task = ring_buffer[buffer_start];
  buffer_start = (buffer_start + 1) % MAX_TASK_NUMBER;
  return task;
}

void put_back_current_task()
{
  ring_buffer_insert(current_task);
}

void update_state(task_t *task)
{
  if (task->state == TASK_RUNNING)
    return;

  if (task->state == TASK_WAITING)
  {
    bool wait_done = update_wait(task);
    if (wait_done)
      return;
    // Wait not done, select another process.
  }
  if (task->state == TASK_DONE)
  {
    if (!any_task_waiting_on(task->pid))
      cleanup_task(task);
  }
}

void find_next_active_task(void)
{
  int32_t initial_buffer_start = buffer_start;
  do
  {
    current_task = ring_buffer_remove();
    update_state(current_task);
    if (current_task->state == TASK_RUNNING)
      return;
    put_back_current_task();
  } while (buffer_start != initial_buffer_start);
  printf("All tasks done or waiting!\n");
}

// Schedule the next task, but don't insert current
// task back into buffer. This means current task 
// will no longer be scheduled.
void schedule_without_insertion(void)
{
  find_next_active_task();
  restore_errno();
  return_to_user_mode = 0;
}

/*
* schedule is called in two cases:
* 1. After a timer interrupt by interrupt_handler.S
* 2. Through a syscall (exit/yield/wait)
* 
* In both cases schedule selects the next task and loads it into
* current_task. Dispatching (switching execution context) is done
* for case 1 in "load_current_task_state" in interrupt_handler.S
* while in case 2 it is done after handling the syscall in
* syscall_handler.S, when return_to_user_mode is set to 0.
*/
void schedule(void)
{
  store_errno();
  put_back_current_task();
  find_next_active_task();
  restore_errno();
  return_to_user_mode = 0;
}

void start_scheduler(void)
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

void rebuild_ring_buffer(void)
{
  int buffer_position = 0;
  buffer_start = 0;
  for (int i = 0; i < MAX_TASK_NUMBER; i++)
  {
    if (tasks[i].valid)
    {
      ring_buffer[buffer_position] = &tasks[i];
      buffer_position++;
    }
  }
  buffer_end = buffer_position;
}

int32_t insert_task(task_t *new_task)
{
  return ring_buffer_insert(new_task);
}

void
reset_timer()
{
  timer_start(TIMER_LOAD_VALUE);
}

void do_yield(void)
{
  reset_timer();
  schedule();
}

void print_ring_buffer_debug_info(void)
{
  printf("------------\n----Scheduling-Buffer----\n");
  printf("Buffer start: %i, Buffer end: %i\n", buffer_start, buffer_end);
  int tasksbufferinfo[MAX_TASK_NUMBER] = {0};
  for (int i = 0; i < MAX_TASK_NUMBER; i++)
  {
    if (ring_buffer[i] != 0)
    {
      tasksbufferinfo[i] = ring_buffer[i]->pid;
    }
  }
  printf("[%i][%i][%i][%i][%i][%i][%i][%i]\n",
         tasksbufferinfo[0], tasksbufferinfo[1], tasksbufferinfo[2], tasksbufferinfo[3],
         tasksbufferinfo[4], tasksbufferinfo[5], tasksbufferinfo[6], tasksbufferinfo[7]);
  printf("[%i][%i][%i][%i][%i][%i][%i][%i]\n",
         tasksbufferinfo[8], tasksbufferinfo[9], tasksbufferinfo[10], tasksbufferinfo[11],
         tasksbufferinfo[12], tasksbufferinfo[13], tasksbufferinfo[14], tasksbufferinfo[15]);
}
