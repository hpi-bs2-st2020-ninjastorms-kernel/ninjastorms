
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

unsigned int syscall_handler();

enum SYSCALL_NUMBER {
    ZERO_SYSCALL = 0,
    CREATE_PROCESS = 1,
    EXIT = 2,
    GET_PID = 3,
    GET_PARENT_PID = 4,
    KILL = 5,
    IS_PREDECESSOR = 6,
    WAIT = 8,
    EXIT_WITH_RESULT = 9,

    //IPC
    OPEN_IPC_BUFFER = 10,
    CLOSE_IPC_BUFFER = 11,
    SEND_TO_IPC_BUFFER = 12,
    READ_IPC_BUFFER = 13,
    LEN_IPC_BUFFER = 14,

    TASKS_INFO = 42,
    SHUTDOWN = 99
}; 
