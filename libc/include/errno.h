
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

enum SYMBOLIC_ERRNO {
    ENOSYS = 1,
    EPERMISSION = 2, //Operation not permitted
    ETOOMANYTASKS = 3, //Too many tasks
    ENOTIMPLEMENTED = 4, //Operation not implemented
    EINVALIDSYSCALLNO = 5, //Syscall no is not recognized
    EBUFFEREMPTY = 6,
    EBUFFERFULL = 7,
    EBUFFERCLOSED = 8,
    EINVALPID = 9 //given PID is not associaated with task
};

extern int errno;
