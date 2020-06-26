
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
#include <stdio.h>

int32_t _set_uid(pid_t target, uid_t uid)
{
    task_t* process = _get_task(target);
    if (process == NULL){
        // errno already set
        return -1;
    }
    process->user = uid;
    return 0;
}

uid_t _get_uid(pid_t target)
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
    return (_get_uid(target) == root);
}

bool current_task_is_super_user()
{
    return (current_task->user == root);
}


bool rights_check_process_on(pid_t caller, pid_t target, rights_t rights)
{
    if((rights & IS_ROOT) == IS_ROOT){
        if(is_super_user(caller))
            return true;
    }
    if((rights & IS_PARENT) == IS_PARENT){
        if(_get_task(target)->parent_pid==caller)
            return true;
    }
    if((rights & REFLEXIVE) == REFLEXIVE){
        if(caller == target)
            return true;
    }
    if((rights & IS_PRED) == IS_PRED){
        if(process_is_descendent_of(target, caller))
            return true;
    }
    if((rights & SAME_USER) == SAME_USER){
        task_t* target_task = _get_task(target);
        task_t* caller_task = _get_task(caller);
        if(caller_task->user == target_task->user)
            return true;
    }
    if((rights & GENERIC) == GENERIC){
        return true;
    }
    errno = EPERMISSION;
    return false;
}

bool rights_check_current_process_on(pid_t target, rights_t rights)
{
    if((rights & IS_ROOT) == IS_ROOT){
        if(current_task_is_super_user())
            return true;
    }
    if((rights & IS_PARENT) == IS_PARENT){
        if(current_task->parent_pid == target)
            return true;
    }
    if((rights & REFLEXIVE) == REFLEXIVE){
        if(current_task->pid == target)
            return true;
    }
    if((rights & IS_PRED) == IS_PRED){
        if(process_is_descendent_of(target, current_task->pid))
            return true;
    }
    if((rights & SAME_USER) == SAME_USER){
        task_t* target_task = _get_task(target);
        if(current_task->user == target_task->user)
            return true;
    }
    if((rights & GENERIC) == GENERIC){
        return true;
    }
    errno = EPERMISSION;
    return false;
}

bool rights_check_current_process(rights_t rights)
{
    if((rights & IS_ROOT) == IS_ROOT){
        if(current_task_is_super_user())
            return true;
    }
    if((rights & GENERIC) == GENERIC){
        return true;
    }
    errno = EPERMISSION;
    return false;
}