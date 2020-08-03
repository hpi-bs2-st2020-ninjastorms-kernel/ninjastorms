
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
 
#define locked   1
#define unlocked 0

// export
.globl lock_mutex
.type lock_mutex STT_FUNC
.globl unlock_mutex
.type unlock_mutex STT_FUNC

lock_mutex:
    LDR r2, =locked
    SWP r1, r2, [r0]       // Swap R2 with location [R0], [R0] value placed in R1.
    CMP r1, r2             // Check if memory value was 'locked'.
    BEQ lock_mutex         // If so, retry immediately.
    BX  lr                 // If not, lock successful, return.

unlock_mutex:
    LDR r1, =unlocked
    STR r1, [r0]           // Write value 'unlocked' to location [R0].
    BX  lr

