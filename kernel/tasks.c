
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

#include "tasks.h"
#include "scheduler.h"
#include "kernel/memory.h"
#include "kernel/utilities.h"
#include "syscall.h"

#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>

#define CPSR_MODE_SVC  0x13
#define CPSR_MODE_USER 0x10

int task_count   = 0;
task_t tasks[MAX_TASK_NUMBER] = {0};
task_t* current_task = &tasks[0];
int next_pid = 1;


void
task_exit(void)
{
    /*
     * This is called when a process has executed all its statements.
     * Thus task_exit() will be executed in user mode, requiring a syscall
     * to properly exit the process
     */
    exit();
}

void
store_errno(void)
{
    current_task->stored_errno=errno;
}

void
restore_errno(void)
{
    errno = current_task->stored_errno;
}

pid_t
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
    task_to_clear->stored_errno=0;

    for(int i=0; i<=IPC_BUFFER_SIZE;i++){
        task_to_clear->ipc_buffer[i]=0;
    }
    task_to_clear->ipc_buffer_start = 0;
    task_to_clear->ipc_buffer_end = 0;
    task_to_clear->ipc_buffer_open = 0;
    
    // zeroing the stack should be done
    // currently the used pid will not be freed
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

unsigned int
calculate_stackbase(int32_t array_position)
{
    return TASK_STACK_BASE_ADDRESS - STACK_SIZE * array_position;
}


/*
 * returns new tasks' pid or -1
 */
pid_t
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
    
    unsigned int stackbase = calculate_stackbase(new_task_pos);
    // push &task_finished
    unsigned int new_pid = init_task(&tasks[new_task_pos], entrypoint, stackbase);
    insert_task(&tasks[new_task_pos]);
    task_count++;
    return new_pid;
}

unsigned int
get_stackbase(pid_t target)
{
    for(int i = 0; i<MAX_TASK_NUMBER; i++){
        if(tasks[i].pid == target){
            return calculate_stackbase(i);
        }
    }
    return 0;
}

task_t* _get_task(pid_t pid)
{
    for(int i=0;i<MAX_TASK_NUMBER;i++){
        if(tasks[i].valid == 1 && tasks[i].pid == pid){
            return &tasks[i];
        }
    }
    errno = EINVALPID;
    return NULL;
} 


void
clone_task(task_t* original, task_t* clone)
{
    for(int i=0;i<=13;i++){
        clone->reg[i]=original->reg[i];
    }
    clone->lr = original->lr;
    clone->pc = original->pc;
    clone->cpsr = original->cpsr;

    //TODO: Handle stack
    uint32_t* original_stack = (uint32_t*) get_stackbase(original->pid);
    uint32_t* new_stack = (uint32_t*) get_stackbase(clone->pid);
    while((uint32_t) original_stack > original->sp){
        *(new_stack++) = *(original_stack++);
    }
    clone->sp = (uint32_t) new_stack;

    clone->stored_errno = original->stored_errno;

    for(int i=0; i<=IPC_BUFFER_SIZE;i++){
        clone->ipc_buffer[i]=original->ipc_buffer[i];
    }
    clone->ipc_buffer_start = original->ipc_buffer_start;
    clone->ipc_buffer_end = original->ipc_buffer_end;
    clone->ipc_buffer_open = original->ipc_buffer_open;
}

void
update_current_task_struct(void)
{
    uint32_t values[3] = {0};
    // Store execution context data to look at it in syscall
    asm(
        "stm %[vals], {lr,pc,sp}^ \n" 
        : :  [vals] "r" (&values)
    );
    current_task->lr = values[0];
    current_task->pc = values[1];
    current_task->sp = values[2];
}

pid_t
do_fork(){
    update_current_task_struct();

    printf("Content of r0 %i\n",current_task->reg[0]);
    pid_t forked_pid = add_task(NULL);
    pid_t original_pid = current_task->pid;
    task_t* new_task = _get_task(forked_pid);

    clone_task(current_task, new_task);

    //set pc to the next instruction
    new_task->pc = current_task->pc;
    
    //Return value for new task
    new_task->reg[0] = 0; 
    return forked_pid;
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
    printf("Tasks-Array --- Task count %i\n",task_count);
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
    print_ring_buffer_debug_info();
    printf("-------------------------------\n");
}


//IPC

void _open_ipc_buffer(size_t size)
{
    // Size will be ignored for now
    current_task->ipc_buffer_open = 1;

    current_task->ipc_buffer_start = 0;
    current_task->ipc_buffer_end = 0;
    printf("Opening ipc\n");
}

int _read_ipc_buffer(void)
{
    if (!current_task->ipc_buffer_open){
        errno = EBUFFERCLOSED;
        return -1;
    }
    if (current_task->ipc_buffer_start == current_task->ipc_buffer_end){
        errno = EBUFFEREMPTY;
        return -1;
    }

    int32_t result = current_task->ipc_buffer[current_task->ipc_buffer_start];
    current_task->ipc_buffer_start =(current_task->ipc_buffer_start +1) % IPC_BUFFER_SIZE;
    //printf("Read %i from buffer of task %i, head now decremented to %i \n",result, current_task->pid, current_task->ipc_buffer_head);
    return result;
}


int _close_ipc_buffer(void)
{
    current_task->ipc_buffer_open = 0;
}

int _send_to_ipc_bufer(int value, pid_t target)
{
    // maybe check at target, if sender is allowed?
    task_t* receiver = (_get_task(target));
    if (receiver == NULL){
        return -1;
    }
    if (!receiver->ipc_buffer_open){
        errno = EBUFFERCLOSED;
        return -1;
    }
    int32_t new_end = (receiver->ipc_buffer_end + 1) % IPC_BUFFER_SIZE;
    if (new_end == receiver->ipc_buffer_start)
    {
        errno = EBUFFERFULL;
        return -1;
    }
    receiver->ipc_buffer[receiver->ipc_buffer_end] = value;
    receiver->ipc_buffer_end = new_end;

    return 0;
}

int _len_ipc_buffer(void)
{
    return current_task->ipc_buffer_end-current_task->ipc_buffer_start;
}