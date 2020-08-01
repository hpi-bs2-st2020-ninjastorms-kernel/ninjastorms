
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

#include <syscall.h>
#include <stdio.h>

#include "user-mode/init.h"
#include "user-mode/examples.h"

void
user_mode_init(void)
{
  printf("User mode initialized with pid: %i\n", get_pid());
  /*
    Call your examples here, like this:

    create_process(&my_example);

    where my_example() is a function that will be used
    to create a process/ task.
    If you want to write new functions in a new file, make
    sure to include it here, add it to "Makefile.am" and
    run the configuration script 
  */

  create_process(&task_wait);
  create_process(&task_calculate);
  create_process(&task_print_info_periodically);
  while (1); //init will run forever
}
