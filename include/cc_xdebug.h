#ifndef XDEBUG_H
#define XDEBUG_H

#include <stdarg.h>
#include <errno.h>
#include <string.h>

#define XDEBUG

//#define XDEBUG_PURE
//#define XDEBUG_CGI

/* For embeded OS should open it to accelerate */
#define XDUMP_MACROS

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

//extern int vasprintf(char **string_ptr, const char *format, ...);

char *xasprintf(const char *format, ...);
char *xvasprintf(const char *fmt, va_list ap);

void xdumphex(const void *buf, unsigned int len);
void _xprintf(FILE *fp,		
              const char *filename,
              const int line,
              const char *funcname,
              const char *fmt, ...);

/*
 * IMPORTANT: Due to the way these color codes are emulated on Windows,
 * write them only using printf(), fprintf(), and fputs(). In particular,
 * do not use puts() or write().
 */
#define COLOR_NORMAL	""
#define COLOR_RESET		"\033[0m"
#define COLOR_BOLD		"\033[1m"
#define COLOR_RED		"\033[31m"
#define COLOR_GREEN		"\033[32m"
#define COLOR_YELLOW	"\033[33m"
#define COLOR_BLUE		"\033[34m"
#define COLOR_MAGENTA	"\033[35m"
#define COLOR_CYAN		"\033[36m"
#define COLOR_BOLD_RED	"\033[1;31m"
#define COLOR_BOLD_GREEN	"\033[1;32m"
#define COLOR_BOLD_YELLOW	"\033[1;33m"
#define COLOR_BOLD_BLUE	"\033[1;34m"
#define COLOR_BOLD_MAGENTA	"\033[1;35m"
#define COLOR_BOLD_CYAN	"\033[1;36m"
#define COLOR_BG_RED	"\033[41m"
#define COLOR_BG_GREEN	"\033[42m"
#define COLOR_BG_YELLOW	"\033[43m"
#define COLOR_BG_BLUE	"\033[44m"
#define COLOR_BG_MAGENTA	"\033[45m"
#define COLOR_BG_CYAN	"\033[46m"

int color_fprintf(FILE *fp, const char *color, const char *fmt, ...);
 
/* xcprintf dump info with color discipline
 * color can be "foreground background textstyle"
 * foreground/background can be "red/green/yellow/blue/magenata/cyan"
 * textstyle can be "bold/empty"
 */
#define xcprintf(color, fmt...) color_fprintf(stdout, color, fmt)

/* xwprintf throws out warning info with red color */
#define xwprintf(fmt...)	xcprintf("red bold", fmt)
/* xwprintf throws out normal info with green color */
#define xiprintf(fmt...)	xcprintf("green bold", fmt)

void xscolor(const char *color);
void xccolor();

/* At least given one parameter for gcc variable length parameter usage 
 * xprintf(), xerror, xdie()... will encounter complaint by GCC.
 */

#ifdef XDUMP_MACROS
#ifdef XDEBUG

/*xprintf dump info to stdout */
#ifdef XDEBUG_CGI
#define xprintf(x...)  _xprintf(NULL, __FILE__, __LINE__, __FUNCTION__, x)
#else
#define xprintf(x...)  _xprintf(stdout, __FILE__, __LINE__, __FUNCTION__, x)
#endif

/*xerror dump info to stderr */
#define xerror(x...)	{\
												_xprintf(stderr, __FILE__, __LINE__, __FUNCTION__, x); \
												color_fprintf(stderr, "red bold", "Tracing: %s\n", errno ? strerror(errno) : "Tracing failed"); \
											}

/* xdie dump info and laster error to stderr and exit with abort */												
#define xdie(x...) 	{	\
												xerror(x); \
												abort();\
										}
#else
#define xprintf(x...) 
#define xerror(x...) {}
#define xdie(x...) {}
#endif
							
#else

void xprintf(const char *s, ...);
void xerror(const char *s, ...);
void xdie(const char *s, ...);
#endif

int xendian();

#endif /* XDEBUG_H */

