
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
#include "syscall.h"
#include <errno.h>

unsigned int syscall(unsigned int number, void* data) 
{
   
    unsigned int ret;

    asm(
        // store arguments in registers
        "mov r0, %[number] \n"  // store number in r0
        "mov r1, %[data] \n"    //   and data in r1

        "svc #0 \n"    // make supervisor call

        "mov %[ret], r0 \n"    // save return value

        : [ret] "=r" (ret)
        : [number] "r" (number),
          [data] "r" (data)
    );

    return ret;
}

int create_process(void * function) 
{
    struct create_process_specification new_process;
    new_process.function = function;
    return syscall(1, &new_process);
}

int exit()
{
    return syscall(2,(void*) 0);
}

unsigned int get_pid(void)
{
    return syscall(3,(void *) 0);
}

unsigned int get_parent_pid(void)
{
    return syscall(4,(void *) 0);
}

int kill(unsigned int target)
{
    struct kill_specification kill_spec;
    kill_spec.pid = target;
    return syscall(5, &kill_spec);
}

int is_predecessor(int child, int pred)
{
    struct is_predecessor_specification is_pred_spec;
    is_pred_spec.child = child;
    is_pred_spec.pred  = pred;
    return syscall(6, &is_pred_spec);
}

int print_tasks_info(void)
{
    return syscall(42,(void *) 0);
}

unsigned int shutdown(void)
{
    return syscall(99,(void *)0);
}


