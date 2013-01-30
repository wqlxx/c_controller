#ifndef CC_LOG_H
#define CC_LOG_H 1

#define LOG_ERR_CC     "log_err_for_cc:" 
#define LOG_INFO_CC    "log_info_for_cc:" 
#define LOG_WARNING_CC "log_warning_for_cc:" 
#define LOG_DEBUG_CC   "log_debug_for_cc:" 

int log_err_for_cc();
int log_warning_for_cc();
int log_info_for_cc();
int log_debug_for_cc();
		
#endif