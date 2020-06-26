
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

int exit(void);

pid_t get_pid(void);

pid_t get_parent_pid(void);

int kill(pid_t target);

uid_t get_uid(void);

int32_t set_uid(pid_t target, uid_t new_uid);

pid_t create_process_with_uid(void * function, uid_t uid);

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
    uid_t uid;
};

struct kill_specification{
    pid_t pid;
    //int signal;
};

struct is_predecessor_specification{
    pid_t child;
    pid_t pred;
};

struct set_uid_specification{
    pid_t target;
    uid_t uid;
};

struct open_ipc_buffer_specification{
    size_t size;
};

struct send_to_ipc_buffer_specification{
    int value;
    pid_t target;
};