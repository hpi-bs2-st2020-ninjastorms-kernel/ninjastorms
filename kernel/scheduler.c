
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
#include "kernel/utilities.h"
#include "kernel/memory.h"
#include "kernel/drivers/timer.h"
#include "kernel/interrupt.h"
#include "kernel/interrupt_handler.h"
#include "syscall.h"

#include <errno.h>
#include <stdio.h>

#define CPSR_MODE_SVC  0x13
#define CPSR_MODE_USER 0x10

#if BOARD_VERSATILEPB
#  define TIMER_LOAD_VALUE 0x2000
#endif

#if BOARD_EV3
#  define TIMER_LOAD_VALUE 0x10000
#endif

#define MAX_TASK_NUMBER 16

int task_count   = 0;
int buffer_start = 0;
int buffer_end   = 0;
int isRunning    = 0;
task_t tasks[MAX_TASK_NUMBER] = {0};
task_t* ring_buffer[MAX_TASK_NUMBER] = { 0 };
task_t* current_task = &tasks[0];
int next_pid = 1;

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

void task_exit()
{
    /*
     * This is called when a process has executed all its statements.
     * Thus task_exit() will be executed in user mode, requiring a syscall
     * to properly exit the process
     */
    exit();
}

int
init_task (task_t *task, void *entrypoint, unsigned int stackbase)
{
  int i;
  for(i = 0; i<13; i++)
    task->reg[i] = i;

  task->sp = stackbase;
  task->lr = (unsigned int) &task_exit;
  task->pc = (unsigned int) entrypoint;


  task->cpsr = CPSR_MODE_USER;
  
  unsigned int new_pid = next_pid++;
  
  task->pid = new_pid;
  if(new_pid == 1){
      //init process
      task->parent_pid = 1;
  } else {
      task->parent_pid = current_task->pid;
  }
  
  
  task->valid = 1;
  
  return new_pid;
}

void
schedule_after_exit(void)
{
    current_task = ring_buffer_remove();
    printf("New task will be Task %i",current_task->pid);
    load_current_task_state();
}

void
clear_task(task_t* task_to_clear)
{
    // should be replaced by memset(task_to_clear,0,size_of(task_t));
    task_to_clear->valid = 0; //should be enough, but let's clean it nontheless
    for(int i=0;i<=13;i++){
        task_to_clear->reg[i]=0;
    }
    task_to_clear->lr = 0;
    task_to_clear->pc = 0;
    task_to_clear->sp = 0;
    task_to_clear->cpsr = 0;
    task_to_clear->pid = 0;
    task_to_clear->parent_pid=0;
    
    // zeroing the stack should be done
    //currently the used pid will not be freed
}


void
reparent_tasks(int killed_pid){
    
    // All tasks that have killed_pid as parent will now have 1 as parent (init)
    for(int i=0;i<MAX_TASK_NUMBER;i++){
        if(tasks[i].valid == 1 && tasks[i].parent_pid == killed_pid){
            tasks[i].parent_pid = 1;
        }
    }
}

void
exit_current_task(void)
{
    task_t* task_to_kill = current_task;
    int pid_to_kill = current_task->pid;
    printf("Task %i will be exited\n",pid_to_kill);
    
    clear_task(task_to_kill);
    
    reparent_tasks(pid_to_kill);
    
    task_count--;
    schedule_after_exit();
}

int
next_free_tasks_position(void)
{
    for(int i=0;i<MAX_TASK_NUMBER;i++){
        if(!tasks[i].valid){
            return i;
        }
    }
    return -1;
}

/*
 * returns new tasks' pid or -1
 */
int
add_task (void *entrypoint)
{
  if (!is_privileged()){
      errno = EPERMISSION;
      return -1;
  }
  if (task_count >= MAX_TASK_NUMBER){
    errno = ETOOMANYTASKS;
    return -1;
  }
  
  int new_task_pos = next_free_tasks_position();

  unsigned int stackbase = TASK_STACK_BASE_ADDRESS - STACK_SIZE * new_task_pos;
  // push &task_finished
  unsigned int new_pid = init_task(&tasks[new_task_pos], entrypoint, stackbase);
  ring_buffer_insert(&tasks[new_task_pos]);
  task_count++;
  return new_pid;
}


void
schedule (void)
{
  ring_buffer_insert(current_task);
  current_task = ring_buffer_remove();
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

// check if process with pid pred is predecessor (parent of parent ...) with pid child 
int
process_is_descendent_of(int child, int pred)
{
    if(child == pred){
        //question of definition
        return 1;
    }
    int current_parent = -1;
    for(int i=0;i<MAX_TASK_NUMBER;i++){
        if (tasks[i].pid == child){
            current_parent = tasks[i].parent_pid;
            break;
        }
    }
    if (current_parent == -1){
        // pid child is not a task!
        return 0;
    }
    while (! (current_parent==1 || current_parent == pred || current_parent==0)){
        for(int i=0;i<MAX_TASK_NUMBER;i++){
            if (tasks[i].pid == current_parent){
                current_parent = tasks[i].parent_pid;
                break;
            }
        }
    }
    return current_parent == pred;
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

int kill_process(int target)
{
    // Don't use kill on the current task, use exit() instead.
    if(target == current_task->pid){
        return -1;
    }
    task_t* task_to_kill = (void*) 0;
    for(int i=0;i<MAX_TASK_NUMBER;i++){
        if(tasks[i].valid == 1 && tasks[i].pid == target){
            task_to_kill = &tasks[i];
        }
    }
    // Task with that pid does not exist
    if(task_to_kill == (void*) 0){
        return -1;
    }
    
    clear_task(task_to_kill);
    task_count--;
    
    reparent_tasks(target);
    
    //remove task from ring buffer by building new ring buffer
    rebuild_ring_buffer();
    
    return 0;    
}

void
print_task_debug_info (void)
{
    printf("-----------TASKS INFO-----------\n");
    printf("Current process\n----------\nPID:%i  PARENT:%i\n----------\n",
           current_task->pid, current_task->parent_pid);
    printf("Tasks-Array\n");
    char tasksinfo[MAX_TASK_NUMBER] = {0};
    for(int i=0;i<MAX_TASK_NUMBER;i++){
        if (tasks[i].valid==1){
            tasksinfo[i] = tasks[i].pid;
        }
    }
    printf("[%i][%i][%i][%i][%i][%i][%i][%i]\n",
           tasksinfo[0],tasksinfo[1],tasksinfo[2],tasksinfo[3],
           tasksinfo[4],tasksinfo[5],tasksinfo[6],tasksinfo[7]);
    printf("[%i][%i][%i][%i][%i][%i][%i][%i]\n",
           tasksinfo[8],tasksinfo[9],tasksinfo[10],tasksinfo[11],
           tasksinfo[12],tasksinfo[13],tasksinfo[14],tasksinfo[15]);
    printf("------------\n----Scheduling-Buffer----\n");
    printf("Buffer start: %i, Buffer end: %i, Task count %i\n",buffer_start,buffer_end,task_count);
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
    printf("-------------------------------\n");
}
