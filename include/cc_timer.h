/*
 * cc_time functions.
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
#ifndef CC_TIMER_H
#define CC_TIMER_H 1

#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include "cc_basic.h"

/* ECHO TIMEOUT has no exactly define,In trema ,i see it is 60s*/
#define CC_ECHO_MAX_INTERVAL 10 //10 use to as a connection checker
#define CC_FEATURE_REPLY_MAX_INTERVAL 10

int cc_timer_check(struct timeval* start_tv, struct timeval* now_tv);

int cc_timer_start(struct timeval* tv);

int cc_timer_expire(struct timeval* tv);

#endif
