/**
 * nmond_ui_curses.c -- Ncurses user interface elements of nmond
 *  Copyright (c) 2015 Christopher Stoll (https://github.com/stollcri)
 *   (for license, see included LICENSE file)
 */

#include "nmond_ui_curses.h"
#include <ncurses.h>

#define COLOUR if(usecolor) /* Only use this for single line color curses calls */

inline void uiheader(int x, int usecolor, int useblink, char *version, char *hostname, double elapsed, time_t timer)
{
	struct tm *tim = localtime(&timer);

	box(stdscr,0,0);
	mvprintw(x, 1, "nmond");
	mvprintw(x, 7, "%s", version);
	if(useblink) mvprintw(x,15,"[H for help]");
	mvprintw(x, 30, "Hostname=%s", hostname);
	mvprintw(x, 52, "Refresh=%2.0fsecs ", elapsed);
	mvprintw(x, 70, "%02d:%02d.%02d", tim->tm_hour, tim->tm_min, tim->tm_sec);
	wnoutrefresh(stdscr);
}

inline void uiwelcome(int x, int usecolor, struct syshw hw)
{
	WINDOW *padwelcome = NULL;
	padwelcome = newpad(24, MAXCOLS);

	COLOUR wattrset(padwelcome,COLOR_PAIR(2));
	mvwprintw(padwelcome,x+1, 1, "--------------------------------------");
	mvwprintw(padwelcome,x+2, 1, "#    #  #    #   ####   #    #  ##### ");
	mvwprintw(padwelcome,x+3, 1, "##   #  ##  ##  #    #  ##   #  #    #");
	mvwprintw(padwelcome,x+4, 1, "# #  #  # ## #  #    #  # #  #  #    #");
	mvwprintw(padwelcome,x+5, 1, "#  # #  #    #  #    #  #  # #  #    #");
	mvwprintw(padwelcome,x+6, 1, "#   ##  #    #  #    #  #   ##  #    #");
	mvwprintw(padwelcome,x+7, 1, "#    #  #    #   ####   #    #  ##### ");
	mvwprintw(padwelcome,x+8, 1, "--------------------------------------");
	COLOUR wattrset(padwelcome,COLOR_PAIR(0));
	mvwprintw(padwelcome,x+1, 40, "For help type H or ...");
	mvwprintw(padwelcome,x+2, 40, " nmond -?  - hint");
	mvwprintw(padwelcome,x+3, 40, " nmond -h  - full");
	mvwprintw(padwelcome,x+5, 40, "To start the same way every time");
	mvwprintw(padwelcome,x+6, 40, " set the NMON ksh variable");
	COLOUR wattrset(padwelcome,COLOR_PAIR(1));

	mvwprintw(padwelcome,x+10, 3, "x86 %s", hw.cpubrand);
	mvwprintw(padwelcome,x+11, 3, "x86 %d Mhz", (hw.cpufrequency / 1000000));
	mvwprintw(padwelcome,x+12, 3, "x86 ProcessorChips=%d PhyscalCores=%d", 1, hw.physicalcpucount);
	mvwprintw(padwelcome,x+13, 3, "x86 Hyperthreads  =%d VirtualCPUs =%d", hw.hyperthreads, hw.logicalcpucount);

	COLOUR wattrset(padwelcome,COLOR_PAIR(0));
	mvwprintw(padwelcome,x+15, 3, "Use these keys to toggle statistics on/off:");
	mvwprintw(padwelcome,x+16, 3, "   c = CPU        l = CPU Long-term   - = Faster screen updates");
	mvwprintw(padwelcome,x+17, 3, "   m = Memory     j = Filesystems     + = Slower screen updates");
	mvwprintw(padwelcome,x+18, 3, "   d = Disks      n = Network         V = Virtual Memory");
	mvwprintw(padwelcome,x+19, 3, "   r = Resource   N = NFS             v = Verbose hints");
	mvwprintw(padwelcome,x+20, 3, "   k = kernel     t = Top-processes   . = only busy disks/procs");
	mvwprintw(padwelcome,x+21, 3, "   h = more options                   q = Quit");
	pnoutrefresh(padwelcome, 0,0,x,1,LINES-2,COLS-2);
	wnoutrefresh(stdscr);
}