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

.section .text

.globl syscall_dispatcher
.global return_to_user_mode

.globl syscall_handler
.type syscall_handler STT_FUNC

// r0 contains syscall number
// r1 contains address of data
syscall_handler:
    push {fp, lr}
    
    // save current task state (even if the syscall does not need this)
    push {r3}   // save r3 for second part of transfer
    ldr r3, =current_task    // get address of task_struct
    ldr r3, [r3]    // dereference current_task, to get the task_struct
    stm r3!, {r0-r2}    // copy r0-r2 using r3 as counter
    mov r2, r3    // copy r3 to r2 so we are able to store r3
    pop {r3}    // restore r3
    stm r2!, {r3-r12}    // store other general purpose registers
    stm r2!, {sp, lr}^   // store sp and lr from user mode
    str lr, [r2]    // store pc

    // call syscall_dispatcher
    bl syscall_dispatcher

    push {r1}
    ldr r1, =return_to_user_mode
    ldr r1, [r1]
    cmp r1, #0
    beq reload_task

return:
    pop {r1, fp, lr}   // restore link register and frame pointer
    movs pc, lr     // return from svc (return and restore cpsr)

reload_task:
    pop {r1, fp, lr}   // restore stack pointer
    b load_current_task_state
