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

// r0 contains the syscall number.
// r1 contains the address of the data.
syscall_handler:
    push {fp, lr}
    
    // Save current task state (even if the syscall does not need this).
    // To use the stm instruction we need a register to store the destination
    // address. Because we want to store all registers in their original state,
    // we use r3 for registers r0 to r2 and r2 for all other registers.
    push {r3}
    ldr r3, =current_task    // Get the address of the task_struct.
    ldr r3, [r3]    // Dereference current_task to get the task_struct.
    stm r3!, {r0-r2}
    mov r2, r3
    pop {r3}
    stm r2!, {r3-r12}
    stm r2!, {sp, lr}^   // Store sp and lr from user mode.
    str lr, [r2]    // Store pc.

    // Call syscall_dispatcher.
    bl syscall_dispatcher

    // Check if the syscall should return to user mode or load the
    // current_task.
    push {r1}
    ldr r1, =return_to_user_mode
    ldr r1, [r1]
    cmp r1, #0
    beq reload_task

return:
    pop {r1, fp, lr}   // Restore link register and frame pointer.
    movs pc, lr     // Return from svc (return and restore cpsr).

reload_task:
    pop {r1, fp, lr}   // Restore stack pointer.
    b load_current_task_state
