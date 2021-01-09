
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
#include "kernel/hal.h"
#include "kernel/utilities.h"
#include "syscall.h"

#include <errno.h>
#include <stdio.h>
#include <logger.h>
#include <stddef.h>
#include <sys/types.h>

#define CPSR_MODE_SVC 0x13
#define CPSR_MODE_USER 0x10

int32_t task_count = 0;
task_t tasks[MAX_TASK_NUMBER] = {0};
task_t *current_task = &tasks[0];
int32_t next_pid = 1;

/*
* This is called when a process has executed all its statements.
* Thus task_exit() will be executed in user mode, requiring a syscall
* to properly exit the process
*/
void task_exit(int32_t return_value)
{
    exit(return_value);
}

void store_errno(void)
{
    current_task->stored_errno = errno;
}

void restore_errno(void)
{
    errno = current_task->stored_errno;
}

pid_t create_new_pid(void)
{
    return next_pid++;
}

pid_t get_new_parent_pid(task_t *new_process)
{
    // The init process has itself as parent.
    if (new_process->pid == 1)
        return 1;

    return current_task->pid;
}

pid_t init_task(task_t *task, void *entrypoint, uint32_t stackbase)
{
    for (int i = 0; i < 13; i++)
        task->reg[i] = i;

    task->sp = stackbase;
    task->lr = (unsigned int)&task_exit;
    task->pc = (unsigned int)entrypoint;

    task->cpsr = CPSR_MODE_USER;

    task->pid = create_new_pid();
    task->parent_pid = get_new_parent_pid(task);

    task->state = TASK_RUNNING;
    task->valid = 1;

    return task->pid;
}

void clear_task(task_t *task_to_clear)
{
    task_to_clear->valid = 0;

    for (int i = 0; i <= 13; i++)
        task_to_clear->reg[i] = 0;

    task_to_clear->lr = 0;
    task_to_clear->pc = 0;
    task_to_clear->sp = 0;
    task_to_clear->cpsr = 0;
    task_to_clear->pid = 0;
    task_to_clear->parent_pid = 0;
    task_to_clear->stored_errno = 0;

    for (int i = 0; i <= IPC_BUFFER_SIZE; i++)
        task_to_clear->ipc_buffer[i] = 0;

    task_to_clear->ipc_buffer_start = 0;
    task_to_clear->ipc_buffer_end = 0;
    task_to_clear->ipc_buffer_open = 0;

    // The task specific stack is not zeroed.
    // Currently the used pid will not be freed.
}

int32_t clear_all_tasks(void)
{
    task_t *task_to_kill = NULL;
    for (int i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (tasks[i].valid)
        {
            task_to_kill = &tasks[i];
            clear_task(task_to_kill);
        }
    }
    return 0;
}

// Set parent_pid for all tasks that have killed_pid as parent
// to 1, thus setting init as their parent.
void reparent_tasks(pid_t killed_pid)
{
    for (int i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (tasks[i].valid && tasks[i].parent_pid == killed_pid)
            tasks[i].parent_pid = 1;
    }
}

void exit_current_task(void)
{
    // This will leave the task in the state TASK_DONE, until no task
    // is waiting on it anymore, which is updated in scheduler.c/update_state().
    current_task->state = TASK_DONE;
    schedule();
}

void cleanup_task(task_t *task_to_clear)
{
    clear_task(task_to_clear);
    task_count--;
    reparent_tasks(task_to_clear->pid);
    schedule_without_insertion();
}

int32_t next_free_tasks_position(void)
{
    for (int i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (!tasks[i].valid)
            return i;
    }
    return -1;
}

task_t *_get_task(pid_t pid)
{
    for (int i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (tasks[i].valid && tasks[i].pid == pid)
            return &tasks[i];
    }
    errno = EINVALPID;
    return NULL;
}

// Creates a new task from given function pointer
// Returns new tasks' pid or -1 on error
pid_t add_task(void *entrypoint)
{
    if (!is_privileged())
    {
        errno = EPERMISSION;
        return -1;
    }
    if (task_count >= MAX_TASK_NUMBER)
    {
        errno = ETOOMANYTASKS;
        LOG_ERROR("ERROR: Can't add task at %x, as tasks array is already full!", entrypoint);
        print_task_debug_info();
        return -1;
    }

    int new_task_pos = next_free_tasks_position();

    unsigned int stackbase = TASK_STACK_BASE_ADDRESS - STACK_SIZE * new_task_pos;
    unsigned int new_pid = init_task(&tasks[new_task_pos], entrypoint, stackbase);
    insert_task(&tasks[new_task_pos]);

    task_count++;

    return new_pid;
}

// check if process with pid pred is predecessor (parent of parent ...) with pid child
bool process_is_descendent_of(pid_t child, pid_t pred)
{
    if (child == pred)
        return true;

    int current_parent = -1;
    // Get first parent, check if child task exists.
    for (int i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (tasks[i].pid == child)
        {
            current_parent = tasks[i].parent_pid;
            break;
        }
    }
    if (current_parent == -1)
        return false;

    // Look for predecessors until at the top of the hierarchy or pred found.
    while (!(current_parent == 1 || current_parent == pred || current_parent == 0))
    {
        for (int i = 0; i < MAX_TASK_NUMBER; i++)
        {
            if (tasks[i].pid == current_parent)
            {
                current_parent = tasks[i].parent_pid;
                break;
            }
        }
    }
    return current_parent == pred;
}

int32_t kill_process(pid_t target)
{
    // Don't use kill on the current task, use exit() instead.
    if (target == current_task->pid)
        return -1;

    task_t *task_to_kill = _get_task(target);

    // Task with that pid does not exist.
    if (task_to_kill == NULL)
        return -1;

    cleanup_task(task_to_kill);

    // Remove task from ring buffer by building new ring buffer.
    rebuild_ring_buffer();

    return 0;
}

bool any_task_waiting_on(pid_t target)
{
    for (int i = 0; i < MAX_TASK_NUMBER; ++i)
    {
        if (tasks[i].valid && tasks[i].state == TASK_WAITING && tasks[i].waiting_on == target)
            return true;
    }
    return false;
}

// Check if task, in TASK_WAIT state, is done waiting and update accordingly.
bool update_wait(task_t *task)
{
    if (task->state != TASK_WAITING)
        return false;

    task_t *target = _get_task(task->waiting_on);
    if (target->state == TASK_DONE)
    {
        task->state = TASK_RUNNING;
        task->reg[0] = target->result; // Put return value for syscall_handler in r0.
        return true;
    }
    return false;
}

// Set the current task as waiting, waiting on task with pid target.
// Reschedule afterwards.
int32_t do_wait(pid_t target)
{
    task_t *waiting = _get_task(target);
    if (waiting == NULL)
        return -1;
    
    current_task->state = TASK_WAITING;
    current_task->waiting_on = target;
    schedule();
}

void do_exit_with(int32_t result)
{
    task_t *task_to_kill = current_task;
    task_to_kill->result = result;

    exit_current_task();
}

void print_task_debug_info(void)
{
    printf("-----------TASKS INFO-----------\n");
    printf("Current process\n-------------\nPID:%i  PARENT:%i\n-------------\n",
           current_task->pid, current_task->parent_pid);
    printf("Tasks-Array --- Task count %i\n", task_count);
    unsigned int tasksinfo[MAX_TASK_NUMBER] = {0};
    for (int i = 0; i < MAX_TASK_NUMBER; i++)
    {
        if (tasks[i].valid)
            tasksinfo[i] = tasks[i].pid;
    }
    printf("[%i][%i][%i][%i][%i][%i][%i][%i]\n",
           tasksinfo[0], tasksinfo[1], tasksinfo[2], tasksinfo[3],
           tasksinfo[4], tasksinfo[5], tasksinfo[6], tasksinfo[7]);
    printf("[%i][%i][%i][%i][%i][%i][%i][%i]\n",
           tasksinfo[8], tasksinfo[9], tasksinfo[10], tasksinfo[11],
           tasksinfo[12], tasksinfo[13], tasksinfo[14], tasksinfo[15]);
    print_ring_buffer_debug_info();
    printf("-------------------------------\n");
}

// Inter process communication (IPC)

void _open_ipc_buffer(size_t size)
{
    // Size will be ignored for now.
    current_task->ipc_buffer_open = true;

    current_task->ipc_buffer_start = 0;
    current_task->ipc_buffer_end = 0;
    LOG_INFO("Opening ipc");
}

int32_t _read_ipc_buffer(void)
{
    if (!current_task->ipc_buffer_open)
    {
        errno = EBUFFERCLOSED;
        return -1;
    }
    if (current_task->ipc_buffer_start == current_task->ipc_buffer_end)
    {
        errno = EBUFFEREMPTY;
        return -1;
    }

    int32_t result = current_task->ipc_buffer[current_task->ipc_buffer_start];
    current_task->ipc_buffer_start = (current_task->ipc_buffer_start + 1) % IPC_BUFFER_SIZE;

    return result;
}

int32_t _close_ipc_buffer(void)
{
    current_task->ipc_buffer_open = false;
    return 0;
}

int32_t _send_to_ipc_bufer(int32_t value, pid_t target)
{
    task_t *receiver = (_get_task(target));
    if (receiver == NULL)
    {
        errno = EINVALPID;
        return -1;
    }
    if (!receiver->ipc_buffer_open)
    {
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

int32_t _len_ipc_buffer(void)
{
    return current_task->ipc_buffer_end - current_task->ipc_buffer_start;
}
