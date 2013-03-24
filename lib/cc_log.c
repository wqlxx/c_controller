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
#include "cc_log.h"

int log_err_for_cc(char *event)
{
	openlog(LOG_ERR_CC,LOG_CONS|LOG_PID,LOG_USER);
	syslog(LOG_ERR,event);
	closelog();
}

int log_info_for_cc(char *event)
{
	openlog(LOG_INFO_CC,LOG_CONS|LOG_PID,LOG_USER);
	syslog(LOG_INFO,evnet);  //event sprintf(event,"dpid id %s",dpid)
	closelog();
}

int log_warning_for_cc(char *event)
{
	openlog(LOG_WARNING_CC,LOG_CONS|LOG_PID,LOG_USER);
	syslog(LOG_WARNING,evnet);  //event sprintf(event,"dpid id %s",dpid)
	closelog();
}

int log_debug_for_cc(char *event)
{
	openlog(LOG_DEBUG_CC,LOG_CONS|LOG_PID,LOG_USER);
	syslog(LOG_DEBUG,evnet);  //event sprintf(event,"dpid id %s",dpid)
	closelog();
}