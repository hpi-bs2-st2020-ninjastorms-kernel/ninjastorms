
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>

#define MAX_TASK_NUMBER 16
#define IPC_BUFFER_SIZE 16

enum TASK_STATE
{
	TASK_RUNNING,
	TASK_WAITING,
	TASK_DONE
};

struct task_t
{
	uint32_t reg[13];
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
	uint32_t cpsr;

	pid_t pid;
	pid_t parent_pid;

	int32_t result;
	int8_t state;
	pid_t waiting_on;

	uint32_t stored_errno;

	int32_t ipc_buffer[IPC_BUFFER_SIZE];
	bool ipc_buffer_open;
	int8_t ipc_buffer_start;
	int8_t ipc_buffer_end;

	bool valid;
};

typedef struct task_t task_t;

extern task_t *current_task;

extern task_t tasks[MAX_TASK_NUMBER];

pid_t add_task(void *entrypoint, bool is_kernel_mode_task);

void exit_current_task(void);

void cleanup_task(task_t *task_to_clear);

int32_t clear_all_tasks(void);

bool process_is_descendent_of(pid_t child, pid_t pred);

int32_t kill_process(pid_t target);

bool any_task_waiting_on(pid_t target);

bool update_wait(task_t *task);

int32_t do_wait(pid_t target);

void do_exit_with(int32_t result);

void print_task_debug_info(void);

void store_errno(void);

void restore_errno(void);

void _open_ipc_buffer(size_t size);

int32_t _read_ipc_buffer(void);

int32_t _close_ipc_buffer(void);

int32_t _send_to_ipc_bufer(int32_t value, pid_t target);

int32_t _len_ipc_buffer(void);
