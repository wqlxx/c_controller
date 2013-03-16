/* Extended debug processing functions.
 * Copyright (C) 2010 Red_Liu lli_njupt@163.com v1.0
 *
 * This file is part of project net-walker.
 *
 * This file is a(part of) free software; you can redistribute it 
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2, or 
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "include/xdebug.h"

#ifdef XDEBUG
void _xprintf(FILE *fp,
              const char *filename,
              const int line,
              const char *funcname,
              const char *fmt, ...)
{
	char *msg = NULL;
	va_list arg;
	FILE *fd = fp;
	
	/* just try to pinrt out to console */
	if(fd == NULL)
	{
		fd = fopen("/dev/console", "a+");
	  if(fd == NULL)return;
	}
	
	va_start(arg, fmt);
#ifndef XDEBUG_PURE	
	fprintf(fd, "=%s(%d):%s= ", filename, line, funcname);
#endif	
	
	if(vasprintf(&msg, fmt, arg) < 0)
	{
		fprintf(fd, "Memory exhausted!\n");
		goto out;
	}

	fprintf(fd, "%s", msg);
	va_end(arg);
	
out:	
	if(fd && fd != stdout && fd != stderr)
		fclose(fd);
	if(msg)
		free(msg);
		
	return;
}

#ifdef XDUMP_MACROS

/*
 * xprintf, xerror, xdie are moved in xdebug.h
#ifdef XDUMP_H
	#define xprintf(x...)  _xprintf(stderr, __FILE__, __LINE__, __FUNCTION__, x);
	#define xerror(x...)	{\
														_xprintf(stderr, __FILE__, __LINE__, __FUNCTION__, x); \
														fprintf(stderr, "%s", errno ? strerror(errno) : NULL); \
												}													
	#define xdie(x...) 	{	\
																xerror(x...); \
																abort();\
															}
#else
  #define xprintf(x...)
  #define xdie(x...)
#endif	
*/

#else

void xprintf(const char *s, ...)
{
	va_list p;

	va_start(p, s);
	_xprintf(stdout, __FILE__, __LINE__, __FUNCTION__, s, p);	
	va_end(p);	
}

void xerror(const char *s, ...)
{
	va_list p;

	va_start(p, s);
	_xprintf(stderr, __FILE__, __LINE__, __FUNCTION__, s, p);	
	va_end(p);
	
	fprintf(stderr, "Tracing: %s\n", errno ? strerror(errno) : NULL);
}

void xdie(const char *s, ...)
{
	va_list p;

	va_start(p, s);
	xerror(s, p);	
	va_end(p);
	
	abort();
}

#endif

/* Die with an error message if we can't malloc() enough space and do an
 * sprintf() into that space.
 */
char *xasprintf(const char *format, ...)
{
	va_list p;
	int r;
	char *retp;

	va_start(p, format);
	r = vasprintf(&retp, format, p);
	va_end(p);

	if(r < 0)
		xdie("vasprintf\n");

	return retp;
}

char *xvasprintf(const char *fmt, va_list ap)
{
	int r;
	char *retp;

	r = vasprintf(&retp, fmt, ap);	
	if(r < 0)
		xdie("vasprintf\n");

	return retp;
}

/* 
*	A usefull function for DEBUG here.
* Guess... the name Ohh. Even thourh the len is defined as unsigned int
* but should be limited by buf size.
*/
void xdumphex(const void *inbuf, unsigned int len)
{  
	int i = 1, j = 0;   
	int left = len % 16;
	unsigned char *buf = (unsigned char*)inbuf; 
#define PRINT printf	
#ifndef isprint
#define isprint(c) ((c) >= ' ' && (c) <= '~')
#endif
	
	PRINT("%04x  ", 0);   
	for(; i <= len; i++)   
	{   
		PRINT("%02x ", buf[i - 1]);   
   	if(i % 8  == 0)   
   	{   
    	if(((i >> 3) % 2) == 0)   
      {   
      	j = i - 15;   
        for(; j <= i; j++)   
        {   
        	if(isprint(buf[j - 1]))PRINT("%c", buf[j - 1]);   
          else PRINT(".");   

          if(j % 8  == 0)   
          {   
          	if(((j >> 3) % 2) == 0)   
            {   
            	if(i != len)PRINT("\n%04x  ", i >> 4);   
              else  
              {   
              	PRINT("\n");   
                return ;   
            	}   
            }else PRINT(" ");   
          }   
      	}   
      }   
    	else PRINT(" ");   
   	}   
	}
	
	j = left;   
	for(;  j < 16; j++)   
	{   
		if(j == 8 && left != 8)PRINT("    ");   
		else PRINT("   ");   
	}   
	
	j = 0;   
	for(i = len - left; i < len; i++)   
	{   
		if(isprint(buf[i]))PRINT("%c", buf[i]);   
		else PRINT(".");
		if(++j == 8)PRINT(" ");   
	}   
	PRINT("\n");
	
	return ;   
}


/* below funcs are for color print stole from git-1.7.2.2 color.c */
/* dump with colors */
static int parse_color(const char *name, int len)
{
	static const char * const color_names[] = 
	{
		"normal", "black", "red", "green", "yellow",
		"blue", "magenta", "cyan", "white"
	};
	
	char *end;
	int i;
	
	for(i = 0; i < ARRAY_SIZE(color_names); i++)
	{
		const char *str = color_names[i];
		if (!strncasecmp(name, str, len) && !str[len])
			return i - 1;
	}
	
	i = strtol(name, &end, 10);
	if(end - name == len && i >= -1 && i <= 255)
		return i;
		
	return -2;
}

static int parse_attr(const char *name, int len)
{
	static const int attr_values[] = { 1, 2, 4, 5, 7 };
	static const char * const attr_names[] =
	{
		"bold", "dim", "ul", "blink", "reverse"
	};
	
	int i;
	for(i = 0; i < ARRAY_SIZE(attr_names); i++)
	{
		const char *str = attr_names[i];
		if(!strncasecmp(name, str, len) && !str[len])
			return attr_values[i];
	}
	
	return -1;
}

/* try to parse color restrictions: vlaue ([fg [bg]] [attr]... ) 
 * to color code: dst
 */
void color_parse(char *dst, const char *value)
{
	int value_len = strlen(value);
	const char *ptr = value;
	int len = value_len;
	unsigned int attr = 0;
	int fg = -2;
	int bg = -2;

	if (!strncasecmp(value, "reset", len)) {
		strcpy(dst, COLOR_RESET);
		return;
	}

	/* [fg [bg]] [attr]... */
	while (len > 0) {
		const char *word = ptr;
		int val, wordlen = 0;

		while (len > 0 && !isspace(word[wordlen]))
		{
			wordlen++;
			len--;
		}

		ptr = word + wordlen;
		while (len > 0 && isspace(*ptr))
		{
			ptr++;
			len--;
		}

		val = parse_color(word, wordlen);
		if (val >= -1) {
			if (fg == -2) {
				fg = val;
				continue;
			}
			if (bg == -2) {
				bg = val;
				continue;
			}
			goto bad;
		}
		val = parse_attr(word, wordlen);
		if (0 <= val)
			attr |= (1 << val);
		else
			goto bad;
	}

	if(attr || fg >= 0 || bg >= 0)
	{
		int sep = 0;
		int i;

		*dst++ = '\033';
		*dst++ = '[';

		for (i = 0; attr; i++)
		{
			unsigned bit = (1 << i);
			if (!(attr & bit))
				continue;
			attr &= ~bit;
			if (sep++)
				*dst++ = ';';
			*dst++ = '0' + i;
		}
		if (fg >= 0)
		{
			if (sep++)
				*dst++ = ';';
			if (fg < 8)
			{
				*dst++ = '3';
				*dst++ = '0' + fg;
			} 
			else
				dst += sprintf(dst, "38;5;%d", fg);			
		}
		if (bg >= 0) 
		{
			if (sep++)
				*dst++ = ';';
			if (bg < 8)
			{
				*dst++ = '4';
				*dst++ = '0' + bg;
			}
			else
				dst += sprintf(dst, "48;5;%d", bg);			
		}
		*dst++ = 'm';
	}
	*dst = 0;
	return;
	
bad:
	xdie("bad color value for variable '%s'\n", value);
}

static int color_vfprintf(FILE *fp, const char *color_code, const char *fmt,
		va_list args, const char *trail)
{
	int r = 0;

	if (*color_code)
		r += fprintf(fp, "%s", color_code);
	r += vfprintf(fp, fmt, args);
	if (*color_code)
		r += fprintf(fp, "%s", COLOR_RESET);
	if (trail)
		r += fprintf(fp, "%s", trail);
	
	/* add this line for color strictly outputing */
	fflush(fp);
	return r;
}

int color_fprintf(FILE *fp, const char *color, const char *fmt, ...)
{
	char color_code[16] = "";
	va_list args;
	int r;
	va_start(args, fmt);
	color_parse(color_code, color);
	r = color_vfprintf(fp, color_code, fmt, args, NULL);
	va_end(args);
	return r;
}

/* set the color for current terminal */
void xscolor(const char *color)
{
	char color_code[16] = "";
	color_parse(color_code, color);
	
	if(*color_code)
		fprintf(stdout, "%s", color_code);	
}

/* clean the color for current terminal */
void xccolor()
{
	fprintf(stdout, "%s", COLOR_RESET);		
}

#endif /* XDEBUG */

