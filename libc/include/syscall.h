
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

unsigned int syscall(unsigned int number, void* data);

pid_t create_process(void * function); 

void exit(int result);

pid_t get_pid(void);

pid_t get_parent_pid(void);

int kill(pid_t target);

int32_t wait_on_pid(pid_t target);

void pass(void);

int is_predecessor(pid_t child, pid_t pred);

int ipc_buffer_open(size_t size);

int ipc_buffer_close(void);

int ipc_buffer_send(int value, pid_t target);

int ipc_buffer_read(void);

int ipc_buffer_length(void);

int print_tasks_info(void);

unsigned int shutdown(void);

struct create_process_specification{
    void * function;
    //int parent_pid;
};

struct kill_specification{
    unsigned int pid;
    //int signal;
};

struct wait_specification{
    pid_t target;
};

struct exit_specification{
    int value;
};

struct is_predecessor_specification{
    int child;
    int pred;
};

struct open_ipc_buffer_specification{
    size_t size;
};

struct send_to_ipc_buffer_specification{
    int value;
    pid_t target;
};