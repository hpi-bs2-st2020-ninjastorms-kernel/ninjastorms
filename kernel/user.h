
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

#define root 0

#include <sys/types.h>

#define IS_ROOT   0b00000001
#define IS_PARENT 0b00000010
#define IS_PRED   0b00000100
#define REFLEXIVE 0b00001000
#define SAME_USER 0b00010000
#define GENERIC   0b10000000

typedef int8_t rights_t;

int32_t _set_uid(pid_t target, uid_t uid);

uid_t _get_uid(pid_t target);

bool is_super_user(pid_t target);

bool rights_check_process_on(pid_t caller, pid_t target, rights_t rights);

bool rights_check_current_process_on(pid_t target, rights_t rights);

bool rights_check_current_process(rights_t rights);
