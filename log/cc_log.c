#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "cc_log.h"

int log_err_for_cc()
{
	openlog(LOG_ERR_CC,LOG_CONS|LOG_PID,LOG_USER);
	syslog(LOG_ERR,"CC is in front of a error");
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