#ifndef NMOND_UI_CURSES_H
#define NMOND_UI_CURSES_H

#include "sysinfo.h"
#include <ncurses.h>
#include <sys/time.h>

/**
 * nmond_ui_curses.h -- Ncurses user interface elements of nmond
 *  Copyright (c) 2015 Christopher Stoll (https://github.com/stollcri)
 *   (for license, see included LICENSE file)
 */

#define MAXROWS 256
#define MAXCOLS 150

#define MSG_VERSION "nmond version %s\n"
#define MSG_INF_GEN_DISKG "Generating disk group file from lsblk output to file: \"auto\"\n"
#define MSG_INF_AUTOF_CMD "Create auto file command was: %s\n"
#define MSG_INF_AUTOFSTAT "Creating auto file returned a status of %d\n"
#define MSG_INF_IGNORINGD "nmond: ignoring -D (extended disk stats) as -g filename is missing\n"
#define MSG_INF_OUTFILENM "nmond: output filename=%s\n"
#define MSG_WRN_NOT_SHOWN "Warning: Some Statistics may not shown"
#define MSG_WRN_OVER_MIND "nmond: ignoring -d %d option as the minimum is %d\n"
#define MSG_ERR_BAD_SHELL "ERROR nmond: invalid NMON_ONE_IN shell variable\n"
#define MSG_ERR_CD_FAILED "changing directory failed"
#define MSG_ERR_FAILEDDIR "Directory attempted was:%s\n"
#define MSG_ERR_NOPENFILE "nmond: failed to open output file"

extern void uiheader(int*, int, int, char*, char*, double, time_t);
extern void uiwelcome(WINDOW**, int*, int, int, int, struct syshw);
extern void uihelp(WINDOW**, int*, int, int);
extern void uisys(WINDOW**, int*, int, int, struct syskern);
extern void uiverbose(WINDOW**, int*, int);
extern void uicpu(WINDOW**, int*, int, int, int, struct sysres, int);
extern void uicpulong(WINDOW **padlong, int*, int, int, int*, int, struct sysres);

#endif
