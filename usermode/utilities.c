
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

#include <errno.h>
#include <sys/types.h>
#include <syscall.h>

#include "utilities.h"

int32_t read_ipc_buffer_and_block(void)
{
    int DELAY = 50000;
    int result = 0;
    do{
        result = ipc_buffer_read();
        if (result == -1){
            for(int i=0; i<DELAY; ++i);
        }
    } while(result == -1 && errno == EBUFFEREMPTY);
    return result;
}

int32_t read_ipc_buffer_and_check(int32_t* validity)
{
    *validity = 1;
    int32_t result = ipc_buffer_read();
    if(result == -1 && errno != 0){
        // The result is not literally "-1", but indicates an error occured (errno is set to 0 by ipc_buffer_read())
        // thus the return value is not valid
        *validity = 0;
    }
    return result;
}

void sleep(uint64_t cycles)
{
    for (long i = 0; i < cycles; ++i);
}