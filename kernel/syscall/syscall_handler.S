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

#include "syscall_dispatcher.h"

.section .text

.globl syscall_handler
.type syscall_handler STT_FUNC

// r0 contains syscall number
// r1 contains address of data
syscall_handler:
    // save current task state

    // fp, local variables
    push {r11, lr}
    add r11, sp, #4
    
    // call syscall_dispatcher
    bl syscall_dispatcher

    // return 
    sub sp, fp, #4  // discard locals
    pop {fp, lr}   // restore link register and frame pointer
    movs pc, lr     // return from svc (return and restore cpsr)
