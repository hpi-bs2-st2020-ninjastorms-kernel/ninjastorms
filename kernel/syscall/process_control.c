
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

#include "process_control.h"
#include "kernel/tasks.h"
#include "kernel/utilities.h"
#include "kernel/scheduler.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <syscall.h>

pid_t create_process_dispatch(void *data)
{
    struct create_process_specification spec = *((struct create_process_specification *)data);
    return add_task(spec.function, false);
}

pid_t get_pid_dispatch(void *data)
{
    return current_task->pid;
}

pid_t get_parent_pid_dispatch(void *data)
{
    return current_task->parent_pid;
}

int32_t exit_dispatch(void *data)
{
    struct exit_specification spec = *((struct exit_specification *)data);
    do_exit_with(spec.value);
    return 0;
}

int32_t kill_dispatch(void *data)
{
    struct kill_specification spec = *((struct kill_specification *)data);
    pid_t target = spec.pid;
    if (target == current_task->pid)
    {
        printf("Do not call kill() on yourself! Use exit() instead.\n");
        return -1;
    }
    if (has_rights(current_task->pid, target))
    {
        return kill_process(target);
    }
    errno = EPERMISSION;
    return -1;
}

uint32_t is_predecessor_dispatch(void *data)
{
    struct is_predecessor_specification spec = *((struct is_predecessor_specification *)data);
    int result = process_is_descendent_of(spec.child, spec.pred);
    return result;
}

int32_t wait_dispatch(void *data)
{
    struct wait_specification spec = *((struct wait_specification *)data);
    return do_wait(spec.target);
}

int32_t pass_dispatch(void *data)
{
    do_pass();
    return 0;
}
