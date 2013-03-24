/*
 * cc_log functions.
 *
 * Author: qiang wang <wqlxx@yahoo.com.cn>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef CC_LOG_H
#define CC_LOG_H 1

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#define LOG_ERR_CC     "log_err_for_cc:" 
#define LOG_INFO_CC    "log_info_for_cc:" 
#define LOG_WARNING_CC "log_warning_for_cc:" 
#define LOG_DEBUG_CC   "log_debug_for_cc:" 

int log_err_for_cc(char* event);
int log_warning_for_cc(char* event);
int log_info_for_cc(char* event);
int log_debug_for_cc(char* event);
		
#endif