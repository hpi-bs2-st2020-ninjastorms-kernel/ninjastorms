
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
#  include <config.h>
#endif

#include <sys/types.h>

#define MAX_TASK_NUMBER 16

struct task_t
{
  // r01..r12, sp, lr, pc
	unsigned int reg[13];
	unsigned int sp;
	unsigned int lr;
	unsigned int pc;
	unsigned int cpsr;
    
    pid_t pid;
    pid_t parent_pid;
    
    char valid; //used for navigating the array
};
typedef struct task_t task_t;

extern task_t *current_task;

extern task_t tasks[MAX_TASK_NUMBER];

pid_t add_task (void *entrypoint);

void exit_current_task(void);

int process_is_descendent_of(int child, int pred);

int kill_process(int target);

void print_task_debug_info (void);
