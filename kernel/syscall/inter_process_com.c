
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

#include "syscall_handler.h"
#include "inter_process_com.h"
#include <syscall.h>
#include "kernel/tasks.h"

#include <errno.h>
#include <sys/types.h>

int32_t open_ipc_buffer_dispatch(void *data)
{
    // Size will be discarded for now
    // Otherwise: cast struct
    _open_ipc_buffer(0);
    return 0;
}

int32_t close_ipc_buffer_dispatch(void *data)
{
    _close_ipc_buffer();
    return 0;
}

int32_t send_to_ipc_buffer_dispatch(void *data)
{
    struct send_to_ipc_buffer_specification spec = *((struct send_to_ipc_buffer_specification *)data);
    int32_t value = spec.value;
    pid_t target = spec.target;
    return _send_to_ipc_bufer(value, target);
}

int32_t read_ipc_buffer_dispatch(void *data)
{
    return _read_ipc_buffer();
}

int32_t length_ipc_buffer_dispatch(void *data)
{
    return _len_ipc_buffer();
}
