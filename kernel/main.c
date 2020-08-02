
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

#include "main.h"

#include "kernel/scheduler.h"
#include "kernel/tasks.h"
#include "user-mode/init.h"

#include <stdio.h>

void print_system_info(void)
{
  char shuriken[] =
      "                 /\\\n"
      "                /  \\\n"
      "                |  |\n"
      "              __/()\\__\n"
      "             /   /\\   \\\n"
      "            /___/  \\___\\\n";
  puts("This is ninjastorms OS");
  puts("  shuriken ready");
  puts(shuriken);
}

int kernel_main(void)
{
  print_system_info();

  add_task(&user_mode_init);
  start_scheduler();

  puts("All done. ninjastorms out!");

  return 0;
}
