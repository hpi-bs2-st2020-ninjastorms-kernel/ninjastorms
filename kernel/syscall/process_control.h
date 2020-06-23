 
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

pid_t create_process_dispatch(void* data);

int32_t exit_dispatch(void* data);

pid_t get_pid_dispatch(void* data);

pid_t get_parent_pid_dispatch(void* data);

int32_t kill_dispatch(void* data);

uint32_t is_predecessor_dispatch(void* data);