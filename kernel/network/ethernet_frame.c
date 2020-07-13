
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

#include "ethernet_frame.h"
#include <sys/types.h>

uint64_t
get_source_mac(ethernet_frame_t *frame) 
{
  uint64_t source_mac = *(uint64_t*)frame->source_mac;
  return source_mac & 0x0000FFFFFFFFFFFF;
}

uint64_t
get_dest_mac(ethernet_frame_t *frame) 
{
  uint64_t dest_mac = *(uint64_t*)frame->dest_mac;
  return dest_mac & 0x0000FFFFFFFFFFFF;
}
