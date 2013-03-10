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

#include "cc_timer.h"

#define CC_CONVERT 1000000

/*
 * return 0: it's not timeout
 * return 1: it's timeout
 * TO check whether the timer is out of deadline
 */
int
cc_timer_check(struct timeval* start_tv, struct timeval* now_tv)
{
	long sec_;
	long usec_;
	long time_;

	sec_ = now_tv->tv_sec - start_tv->tv_sec;
	usec_ = now_tv->tv_usec - start_tv->tv_usec;

	time_ = sec_ + usec_/CC_CONVERT;

	if( time_ > CC_ECHO_MAX_INTERVAL )
		return CC_ERROR;

	return CC_SUCCESS;
}


/*
 *param: tv come from the xid table
 *TO write down the time call this function.and set as the begin time for a xid.
 *next time it got the same xid and we can use it as a value to check whether it is timeout
 */
int
cc_timer_start(struct timeval* tv)
{
	int ret;
	ret = gettimeofday(tv,NULL);
	if( ret == -1)
		return CC_ERROR;
	return CC_SUCCESS;
}

/*
 *param: tv come from the xid table
 *TO expire the begin time of a xid, and set it as zero
 */
int
cc_timer_expire(struct timeval* tv)
{
	tv->sec = 0;
	tv->usec = 0;
	
	return CC_SUCCESS;
}


