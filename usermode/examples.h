
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

void task_a(void);

void task_b(void);

void task_c(void);

void task_d(void);

void task_e(void);

void task_receiver(void);

void task_sender(void);

int task_calculate(void);

void task_wait(void);

int task_exit_test(void);

int task_pass_a(void);

int task_pass_b(void);

void task_print_info_periodically(void);

void task_recursive_exit(void);

void task_mutex_a (void);

void task_mutex_b (void);

void task_orchestrate_example(void);