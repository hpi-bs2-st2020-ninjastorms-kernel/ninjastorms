
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

#include "user.h"

#include "tasks.h" 
#include <sys/types.h>
#include <errno.h>
#include <stddef.h>

int32_t set_uid(pid_t target, uid_t uid)
{
    task_t* process = _get_task(target);
    if (process == NULL){
        // errno already set
        return -1;
    }
    process->user = uid;
}

uid_t get_uid(pid_t target)
{
    task_t* process = _get_task(target);
    if (process == NULL){
        // errno already set
        return -1;
    }
    return process->user;
}

bool is_super_user(pid_t target)
{
    return (get_uid(target) == root);
}

/*
 * check if calling_process has rights for actions on process target
 */
bool has_rights(pid_t calling_process, pid_t target)
{
    // same process
    if(calling_process == target){
        return true;
    }
    // super user
    if(is_super_user(target)){
        return true;
    }
    // target is child of calling_process
    if(process_is_descendent_of(target,calling_process)){
        return true;
    }
    return false;
}
