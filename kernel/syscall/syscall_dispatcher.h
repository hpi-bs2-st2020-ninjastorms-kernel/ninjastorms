
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

int32_t syscall_dispatcher(uint32_t syscall_number, void *data);

// If this is not zero, when a syscall is handled and the control
// flow returns to syscall_handler(unsigned int number, void *data) after
// syscall_dispatch, instead of returning to the user mode, the task in
// current_task is loaded. This enables easy implementation of syscalls that
// change the currently active user mode process, like exit, wait and yield.
extern int8_t return_to_user_mode;
