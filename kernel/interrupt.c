
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

#include "interrupt.h"
#include "syscall/syscall_handler.h"

#include "kernel/hal.h"
#include "kernel/drivers/interrupt.h"
#include "kernel/time.h"
#include "kernel/interrupt_handler.h"


// Builds the interrupt vector table.
void setup_ivt(void)
{
  // This is the machine word for "ldr pc, [pc, #20]".
  unsigned int ldr_pc_pc_20 = 0xe59ff014;

  // The processor jumps to one of these addresses whenever an interrupt
  // arrives. Because the word contains the "ldr pc, [pc, #20]" instruction,
  // it loads the address specified at byte offset 20, where the actual address
  // of the appropriate interrupt handler is stored.
  *(unsigned int *)(IVT_OFFSET + 0x00) = 0;            //TODO: reset
  *(unsigned int *)(IVT_OFFSET + 0x04) = ldr_pc_pc_20; // 0x20 undefined instruction
  *(unsigned int *)(IVT_OFFSET + 0x08) = ldr_pc_pc_20; // 0x24 software interrupt
  *(unsigned int *)(IVT_OFFSET + 0x0c) = ldr_pc_pc_20; // 0x28 prefetch abort
  *(unsigned int *)(IVT_OFFSET + 0x10) = ldr_pc_pc_20; // 0x2c data abort
  *(unsigned int *)(IVT_OFFSET + 0x14) = ldr_pc_pc_20; // 0x30 reserved
  *(unsigned int *)(IVT_OFFSET + 0x18) = ldr_pc_pc_20; // 0x34 IRQ
  *(unsigned int *)(IVT_OFFSET + 0x1c) = ldr_pc_pc_20; // 0x38 FIQ

  // Addresses of interrupt handlers.
  *(unsigned int *)(IVT_OFFSET + 0x20) = (unsigned int)0;
  // ATTENTION: don't use software interrupts in supervisor mode:
  *(unsigned int *)(IVT_OFFSET + 0x24) = (unsigned int)&syscall_handler;
  *(unsigned int *)(IVT_OFFSET + 0x28) = (unsigned int)0;
  *(unsigned int *)(IVT_OFFSET + 0x2c) = (unsigned int)0;
  *(unsigned int *)(IVT_OFFSET + 0x30) = (unsigned int)0;
  *(unsigned int *)(IVT_OFFSET + 0x34) = (unsigned int)&irq_handler_timer;
  *(unsigned int *)(IVT_OFFSET + 0x38) = (unsigned int)0;

  set_ivt_location();
}

void setup_irq_stack(void)
{
  asm volatile(
      "mrs  r0, cpsr \n"
      "bic  r0, #0x1f \n"                // Clear mode bits
      "orr  r0, #0x12 \n"                // Select IRQ mode
      "msr  cpsr, r0 \n"                 // Enter IRQ mode
      "mov  sp, %[irq_stack_address] \n" // set stack pointer
      "bic  r0, #0x1f \n"                // Clear mode bits
      "orr  r0, #0x13 \n"                // Select SVC mode
      "msr  cpsr, r0 \n"                 // Enter SVC mode
      :
      : [ irq_stack_address ] "r"(IRQ_STACK_ADDRESS));
}

void enable_irq(void)
{
  asm(
      "mrs  r1, cpsr\n"
      "bic  r1, r1, #0x80\n"
      "msr  cpsr_c, r1\n"
      );
}

void init_interrupt_handling(void)
{
  setup_ivt();
  setup_irq_stack();
  init_interrupt_controller();
  enable_irq();
}
