
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
#pragma once

#include <sys/types.h>

int32_t syscall(uint32_t number, void *data);

pid_t create_process(void *function);

void exit(int32_t result);

pid_t get_pid(void);

pid_t get_parent_pid(void);

int32_t kill(pid_t target);

int32_t wait_on_pid(pid_t target);

void yield(void);

int32_t is_predecessor(pid_t child, pid_t pred);

int32_t ipc_buffer_open(size_t size);

int32_t ipc_buffer_close(void);

int32_t ipc_buffer_send(int32_t value, pid_t target);

int32_t ipc_buffer_read(void);

int32_t ipc_buffer_length(void);

int32_t print_tasks_info(void);

int32_t shutdown(void);

// Structs used to capsulate data,
// send address to struct to kernel mode.

struct create_process_specification
{
    void *function;
};

struct kill_specification
{
    uint32_t pid;
};

struct wait_specification
{
    pid_t target;
};

struct exit_specification
{
    int32_t value;
};

struct is_predecessor_specification
{
    pid_t child;
    pid_t pred;
};

struct open_ipc_buffer_specification
{
    size_t size;
};

struct send_to_ipc_buffer_specification
{
    int32_t value;
    pid_t target;
};
