
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

#include "syscall_dispatcher.h"
#include "kernel/tasks.h"
#include "kernel/interrupt_handler.h"

#include <syscall.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include "inter_process_com.h"
#include "process_control.h"

int8_t return_to_user_mode = 0;

// Do not change these numbers without changing them in the syscall.c library code.
enum SYSCALL_NUMBER
{
    ZERO_SYSCALL = 0,
    CREATE_PROCESS = 1,
    EXIT = 2,
    GET_PID = 3,
    GET_PARENT_PID = 4,
    KILL = 5,
    IS_PREDECESSOR = 6,
    //FORK = 7, (Not implemented)
    WAIT = 8,
    YIELD = 9,

    //IPC
    OPEN_IPC_BUFFER = 10,
    CLOSE_IPC_BUFFER = 11,
    SEND_TO_IPC_BUFFER = 12,
    READ_IPC_BUFFER = 13,
    LEN_IPC_BUFFER = 14,

    TASKS_INFO = 42,
    SHUTDOWN = 99
};

uint32_t syscall_zero_dispatch(void *data)
{
    puts("This is not a real syscall!\n");
    return 0;
}

int32_t task_info_dispatch(void *data)
{
    print_task_debug_info();
    return 0;
}

uint32_t shutdown_dispatch(void *data)
{
    // Shutdown is more complicated on ARM. Currently only an endless loop.
    clear_all_tasks();
    while (1);
}

// Call the corresponding function for a syscall number. These follow the
// naming scheme <syscall>_dispatch(void *data) and for the most part call
// the kernel mode functions responsible for serving the syscall.
int32_t syscall_dispatcher(uint32_t syscallno, void *data)
{
    return_to_user_mode = 1;
    //printf("Handling syscall %i with data at address %x.\n", syscallno, data);
    switch (syscallno)
    {
    case ZERO_SYSCALL:
        return syscall_zero_dispatch(data);
    case CREATE_PROCESS:
        return create_process_dispatch(data);
    case EXIT:
        return exit_dispatch(data);
    case GET_PID:
        return get_pid_dispatch(data);
    case GET_PARENT_PID:
        return get_parent_pid_dispatch(data);
    case KILL:
        return kill_dispatch(data);
    case IS_PREDECESSOR:
        return is_predecessor_dispatch(data);
    case WAIT:
        return wait_dispatch(data);
    case YIELD:
        return yield_dispatch(data);
    case OPEN_IPC_BUFFER:
        return open_ipc_buffer_dispatch(data);
    case CLOSE_IPC_BUFFER:
        return close_ipc_buffer_dispatch(data);
    case SEND_TO_IPC_BUFFER:
        return send_to_ipc_buffer_dispatch(data);
    case READ_IPC_BUFFER:
        return read_ipc_buffer_dispatch(data);
    case LEN_IPC_BUFFER:
        return length_ipc_buffer_dispatch(data);
    case TASKS_INFO:
        return task_info_dispatch(data);
    case SHUTDOWN:
        return shutdown_dispatch(data);
    default:
        errno = EINVALIDSYSCALLNO;
        return -1;
    }
}
