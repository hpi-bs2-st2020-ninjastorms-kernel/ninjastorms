
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

#include "kernel/drivers/interrupt.h"
#include "kernel/hal.h"
#include "kernel/mmio.h"
#include "kernel/network/e1000.h"


void
set_ivt_location(void)
{
#if BOARD_EV3
    // set V bit in c1 register in cp15 to
    // locate interrupt vector table to 0xFFFF0000
    asm(
      "mrc  p15, 0, r0, c1, c0, 0\n"
      "orr  r0, #0x2000\n"
      "mcr  p15, 0, r0, c1, c0, 0\n");
#endif
}

void
handle_interrupt (void)
{
  uint32_t status = read32((uint32_t) PIC_IRQ_STATUS);
  if(status & PCI3_INTBIT)
    e1000_irq_handler();
  if(status & TIMER1_INTBIT)
    irq_handler_timer();
  if(status & TIMER3_INTBIT)
    irq_handler_clock();
}

void
init_timer_interrupt (void)
{
    #if BOARD_VERSATILEPB
    *PIC_INT_ENABLE |= TIMER1_INTBIT;  // unmask interrupt bit for timer1
    *PIC_INT_ENABLE |= TIMER3_INTBIT;  // unmask interrupt bit for timer3
    #endif
    
    #if BOARD_EV3
    *AINTC_ESR1 |= T64P0_TINT34; // enable timer interrupt
    *AINTC_CMR5 |= 2 << (2*8);   // set channel of timer interrupt to 2
    #endif
}

void
init_pci_interrupt (void)
{
  // enable PCI[0..3] interrupts for primary and secondary interrupt controllers
  *PIC_INT_ENABLE |= PCI0_INTBIT | PCI1_INTBIT | PCI2_INTBIT | PCI3_INTBIT;
  *SIC_ENABLE |= PCI0_INTBIT | PCI1_INTBIT | PCI2_INTBIT | PCI3_INTBIT;
  // forward interrupts from secondary to primary interrupt controller
  *SIC_PIC_EN_SET |= PCI0_INTBIT | PCI1_INTBIT | PCI2_INTBIT | PCI3_INTBIT;
}


void
init_interrupt_controller (void)
{
#if BOARD_VERSATILEPB
  *PIC_INT_ENABLE |= 2; // unmask interrupt bit for software interrupt
#endif

#if BOARD_EV3
  *AINTC_SECR1 = 0xFFFFFFFF;   // clear current interrupts
  *AINTC_GER   = GER_ENABLE;   // enable global interrupts
  *AINTC_HIER |= HIER_IRQ;     // enable IRQ interrupt line
  // 0-1 are FIQ channels, 2-31 are IRQ channels, lower channels have higher priority
#endif
 init_timer_interrupt();
 init_pci_interrupt();
}