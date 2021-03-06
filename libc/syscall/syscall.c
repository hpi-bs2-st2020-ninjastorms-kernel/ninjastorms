
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
#include <syscall.h>
#include <errno.h>
#include <sys/types.h>
#include <stddef.h>

int32_t syscall(unsigned int number, void *data)
{
    unsigned int ret;

    asm(
        // Store arguments in registers.
        "mov r0, %[number] \n" // Store syscall number in r0
        "mov r1, %[data] \n"   // and address of data in r1.

        "svc #0 \n"            // Make supervisor call.

        "mov %[ret], r0 \n"    // Save return value.

        : [ ret ] "=r"(ret)
        : [ number ] "r"(number),
          [ data ] "r"(data)
        );
    return ret;
}

pid_t create_process(void *function)
{
    struct create_process_specification new_process;
    new_process.function = function;
    return syscall(1, &new_process);
}

void exit(int32_t result)
{
    struct exit_specification spec;
    spec.value = result;
    syscall(2, &spec);
}

pid_t get_pid(void)
{
    return syscall(3, NULL);
}

pid_t get_parent_pid(void)
{
    return syscall(4, NULL);
}

int32_t kill(pid_t target)
{
    struct kill_specification kill_spec;
    kill_spec.pid = target;
    return syscall(5, &kill_spec);
}

int32_t is_predecessor(pid_t child, pid_t pred)
{
    struct is_predecessor_specification is_pred_spec;
    is_pred_spec.child = child;
    is_pred_spec.pred = pred;
    return syscall(6, &is_pred_spec);
}

int32_t wait_on_pid(pid_t target)
{
    struct wait_specification spec;
    spec.target = target;
    return syscall(8, &spec);
}

void yield(void)
{
    syscall(9, NULL);
}

// Inter process communication

int32_t ipc_buffer_open(size_t size)
{
    struct open_ipc_buffer_specification open_ipc_spec;
    open_ipc_spec.size = size;
    return syscall(10, &open_ipc_spec);
}

int32_t ipc_buffer_close(void)
{
    return syscall(11, NULL);
}

int32_t ipc_buffer_send(int32_t value, pid_t target)
{
    struct send_to_ipc_buffer_specification send_ipc_spec;
    send_ipc_spec.target = target;
    send_ipc_spec.value = value;
    return syscall(12, &send_ipc_spec);
}

int32_t ipc_buffer_read(void)
{
    /* 
    *  The syscall may return -1.
    *  This can be because an error has occured,
    *  e.g. Buffer not open or Buffer empty
    *  OR the message in the buffer is "-1".
    *  Receiver has to check errno to determine that.
    */
    errno = 0;
    return syscall(13, NULL);
}

int32_t ipc_buffer_length(void)
{
    return syscall(14, NULL);
}

// Debug information

int32_t print_tasks_info(void)
{
    return syscall(42, NULL);
}

// System control

int32_t shutdown(void)
{
    return syscall(99, NULL);
}
