
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

#include "syscall_handler.h"
#include "process_control.h"
#include "kernel/syscall.h"
#include "kernel/tasks.h"
#include "kernel/utilities.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>


pid_t create_process_dispatch(void* data)
{
    struct create_process_specification spec = *((struct create_process_specification*) data);
    int result = add_task(spec.function);
    return result;
}

int32_t exit_dispatch(void* data)
{
    exit_current_task();
    return 0;
}

pid_t get_pid_dispatch(void* data)
{
    return current_task->pid;
}

pid_t get_parent_pid_dispatch(void* data)
{
    return current_task->parent_pid;
}

int32_t kill_dispatch(void* data)
{
    struct kill_specification spec = *((struct kill_specification*) data);
    int target = spec.pid;
    if(target == current_task->pid){
        printf("Do not call kill() on yourself! Use exit() instead.\n");
        return -1;
    }
    if(has_rights(current_task->pid, target)){
        return kill_process(target);
    }
    errno = EPERMISSION;
    return -1;
}

uint32_t is_predecessor_dispatch(void* data)
{
    struct is_predecessor_specification spec = *((struct is_predecessor_specification*) data);
    int result = process_is_descendent_of(spec.child,spec.pred);
    return result;
}