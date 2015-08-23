#ifndef NMOND_UI_CURSES_H
#define NMOND_UI_CURSES_H

#include "sysinfo.h"
#include <sys/time.h>

/**
 * nmond_ui_curses.h -- Ncurses user interface elements of nmond
 *  Christopher Stoll (https://github.com/stollcri), 2015
 */

#define MAXROWS 256
#define MAXCOLS 150

extern void uiheader(int, int, int, char*, char*, double, time_t);
extern void uiwelcome(int, int, struct syshw);

#endif