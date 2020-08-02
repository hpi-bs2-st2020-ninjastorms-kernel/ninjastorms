
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

#include "utilities.h"
#include "tasks.h"

#include <errno.h>
#include <sys/types.h>

// KERNEL UTILITIES

uint32_t get_operating_mode(void)
{
    // Processing modes on ARM
    // usr 0b10000
    // fiq 0b10001 fast interrupt
    // irq 0b10010
    // svc 0b10011 supervisor
    // abt 0b10111
    // und 0b11011 undefined
    // sys 0b11111

    unsigned int current_pcsr = 0;
    asm(
        "mrs r3, cpsr\n"
        "mov %[current_pcsr], r3\n"
        : [ current_pcsr ] "=r"(current_pcsr));
    return current_pcsr & 0x1f;
}

bool is_privileged(void)
{
    return get_operating_mode() != 0b10000;
}

/*
 * check if calling_process has rights for actions on process target
 */
bool has_rights(pid_t calling_process, pid_t target)
{
    // same process
    if (calling_process == target)
    {
        return true;
    }
    // target is child of calling_process
    if (process_is_descendent_of(target, calling_process))
    {
        return true;
    }
    return false;
}
