/* procinfo.h: reporting the time and memory usage of a process.
 *
 * Copyright (c) 2004-2012, Deniz Yuret <denizyuret@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PROCINFO_H__
#define __PROCINFO_H__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define dot(n) (((n)==0) ? (_procinfo_cnt = (fputc('\n', stderr), 0)) :\
		(++_procinfo_cnt && ((n)<64)) ? fputc('.', stderr) :\
		(_procinfo_cnt % ((n)>>2) == 0) ? fprintf(stderr, "%d%%", 25*_procinfo_cnt/(n>>2)) :\
		(_procinfo_cnt % ((n)>>5) == 0) ? fputc('.', stderr) : 0)

unsigned long _procinfo_cnt = 0;

static unsigned long long memory() {
  static char buf[1024];
  FILE *fp;
  unsigned long long mem = 0;
  if ((fp = fopen("/proc/self/stat", "r")) != NULL) {
    if (fgets(buf, 1024, fp)) {
      char *tok = strtok(buf, " ");
      for (int i = 1; i < 23; i++) {
	tok = strtok(NULL, " ");
      }
      if (tok != NULL) {
	mem = atoll(tok);
      }
    }
  }
  return mem;
}

static unsigned long runtime() {
  static time_t t0 = 0;
  time_t t1 = time(NULL);
  if (t0 == 0) t0 = t1;
  return t1-t0;
}

/* Issue: clock() starts giving negative numbers after a while.  I
   checked the header files, CLOCKS_PER_SEC is defined to be 1000000
   and the return value of clock, clock_t, is defined as signed long.
   This means after 2000 seconds things go negative.  Need to use some
   other function for longer programs. 

   Switched to using times, which works with _SC_CLK_TCK defined as
   100 so the overflow should take longer.

   That also didn't work, finally decided to use the time function
   which has a resolution of seconds.
*/

#ifdef __G_LIB_H__

static void my_log_func(const gchar *log_domain,
		 GLogLevelFlags log_level,
		 const gchar *message,
		 gpointer user_data) {
  fprintf(stderr, "[t=%lu m=%llu] %s\n", 
	  runtime(), memory(), message);
}

static void g_message_init()
{
  g_log_set_handler(NULL, G_LOG_LEVEL_MESSAGE, my_log_func, NULL);
  runtime();
}

#endif

#endif
