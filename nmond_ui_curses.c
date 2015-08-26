/**
 * nmond_ui_curses.c -- Ncurses user interface elements of nmond
 *  Copyright (c) 2015 Christopher Stoll (https://github.com/stollcri)
 *   (for license, see included LICENSE file)
 */

#include "nmond_ui_curses.h"

#define COLOUR if(usecolor) /* Only use this for single line color curses calls */

static inline void uibanner(int cols, WINDOW *pad, char *string)
{
	mvwhline(pad, 0, 0, ACS_HLINE,cols-2);
	wmove(pad, 0, 0);
	wattron(pad, A_STANDOUT);
	wprintw(pad, " ");
	wprintw(pad, string);
	wprintw(pad, " ");
	wattroff(pad, A_STANDOUT);
}


static inline void uidisplay(int *xin, int cols, int rows, WINDOW *pad, int lines)
{
	int x = *xin;

	if(x+2+(rows) > lines) {
		pnoutrefresh(pad, 0,0,x,1,lines-2, cols-2);
	} else {
		pnoutrefresh(pad, 0,0,x,1,x+rows+1,cols-2);
	}

	x=x+(rows);
	if(x+4 > lines) {
		mvwprintw(stdscr, lines-1, 1, MSG_WRN_NOT_SHOWN);
	}

	*xin = x;
}

inline void uiheader(int x, int usecolor, int useblink, char *version, char *hostname, double elapsed, time_t timer)
{
	struct tm *tim = localtime(&timer);

	box(stdscr,0,0);
	mvprintw(x, 2, "nmond");
	mvprintw(x, 8, "%s", version);
	if(useblink) {
		mvprintw(x,14,"[H for help]");
	}
	mvprintw(x, 29, "%s", hostname);
	mvprintw(x, 52, "Refresh=%2.0fsecs ", elapsed);
	mvprintw(x, 70, "%02d:%02d.%02d", tim->tm_hour, tim->tm_min, tim->tm_sec);
	wnoutrefresh(stdscr);
}

inline void uiwelcome(WINDOW **padwelcomein, int *xin, int cols, int rows, int usecolor, struct syshw hw)
{
	WINDOW *padwelcome = *padwelcomein;
	if (padwelcome == NULL) {
		return;
	}
	int x = *xin;

	COLOUR wattrset(padwelcome, COLOR_PAIR(2));
	mvwprintw(padwelcome, x+1, 1, "                                         __");
	mvwprintw(padwelcome, x+2, 1, "    ____   ____ ___   ____   ____   ____/ /");
	mvwprintw(padwelcome, x+3, 1, "   / __ \\ / __ `__ \\ / __ \\ / __ \\ / __  / ");
	mvwprintw(padwelcome, x+4, 1, "  / / / // / / / / // /_/ // / / // /_/ /  ");
	mvwprintw(padwelcome, x+5, 1, " /_/ /_//_/ /_/ /_/ \\____//_/ /_/ \\____/   ");
	mvwprintw(padwelcome, x+6, 1, "=======================================    ");
	COLOUR wattrset(padwelcome, COLOR_PAIR(0));
	mvwprintw(padwelcome, x+1, 45, "For help type H or ...");
	mvwprintw(padwelcome, x+2, 45, " nmond -?  - hint");
	mvwprintw(padwelcome, x+3, 45, " nmond -h  - full");
	mvwprintw(padwelcome, x+5, 45, "To start the same way every time");
	mvwprintw(padwelcome, x+6, 45, " set the NMOND shell variable");
	COLOUR wattrset(padwelcome, COLOR_PAIR(1));
	mvwprintw(padwelcome, x+8,  2, "%s", hw.model);
	mvwprintw(padwelcome, x+9,  2, "%s", hw.cpubrand);
	mvwprintw(padwelcome, x+10, 2, "CPU Count    = %2d   Physcal Cores = %d", hw.cpucount, hw.physicalcpucount);
	mvwprintw(padwelcome, x+11, 2, "Hyperthreads = %2d   Virtual CPUs  = %d", hw.hyperthreads, hw.logicalcpucount);
	COLOUR wattrset(padwelcome, COLOR_PAIR(0));
	mvwprintw(padwelcome, x+15, 3, "Use these keys to toggle statistics on/off:");
	mvwprintw(padwelcome, x+16, 3, "   c = CPU        l = CPU Long-term   - = Faster screen updates");
	mvwprintw(padwelcome, x+17, 3, "   m = Memory     j = Filesystems     + = Slower screen updates");
	mvwprintw(padwelcome, x+18, 3, "   d = Disks      n = Network         V = Virtual Memory");
	mvwprintw(padwelcome, x+19, 3, "   r = Resource   N = NFS             v = Verbose hints");
	mvwprintw(padwelcome, x+20, 3, "   k = kernel     t = Top-processes   . = only busy disks/procs");
	mvwprintw(padwelcome, x+21, 3, "   h = more options                   q = Quit");
	pnoutrefresh(padwelcome, 0, 0, x, 1, rows-2, cols-2);
	wnoutrefresh(stdscr);
	*xin = x;
}

inline void uihelp(WINDOW **padhelpin, int *xin, int cols, int rows)
{
	WINDOW *padhelp = *padhelpin;
	if (padhelp == NULL) {
		return;
	}
	int x = *xin;
	uibanner(cols, padhelp, "HELP");
	mvwprintw(padhelp,  1, 5, "key  --- statistics which toggle on/off ---%d",x);
	mvwprintw(padhelp,  2, 5, "h = This help information");
	mvwprintw(padhelp,  3, 5, "r = RS6000/pSeries CPU/cache/OS/kernel/hostname details + LPAR");
	mvwprintw(padhelp,  4, 5, "t = Top Process Stats 1=basic 3=CPU");
	mvwprintw(padhelp,  5, 5, "    u = shows command arguments (hit twice to refresh)");
	mvwprintw(padhelp,  6, 5, "c = CPU by processor             l = longer term CPU averages");
	mvwprintw(padhelp,  7, 5, "m = Memory & Swap stats L=Huge   j = JFS Usage Stats");
	mvwprintw(padhelp,  8, 5, "n = Network stats                N = NFS");
	mvwprintw(padhelp,  9, 5, "d = Disk I/O Graphs D=Stats      o = Disks %%Busy Map");
	mvwprintw(padhelp, 10, 5, "k = Kernel stats & loadavg       V = Virtual Memory");
	mvwprintw(padhelp, 11, 5, "g = User Defined Disk Groups [start nmon with -g <filename>]");
	mvwprintw(padhelp, 12, 5, "v = Verbose Simple Checks - OK/Warnings/Danger");
	mvwprintw(padhelp, 13, 5, "b = black & white mode");
	mvwprintw(padhelp, 14, 5, "--- controls ---");
	mvwprintw(padhelp, 15, 5, "+ and - = double or half the screen refresh time");
	mvwprintw(padhelp, 16, 5, "q = quit                     space = refresh screen now");
	mvwprintw(padhelp, 17, 5, ". = Minimum Mode =display only busy disks and processes");
	mvwprintw(padhelp, 18, 5, "0 = reset peak counts to zero (peak = \">\")");
	mvwprintw(padhelp, 19, 5, "Chrisotpher Stoll, 2015 (https://github.com/stollcri)");
	pnoutrefresh(padhelp, 0, 0, x, 1, rows-2, cols-2);
	uidisplay(&x, cols, 20, padhelp, rows);
	*xin = x;
}

inline void uicpu(WINDOW **padcpuin, int *xin, int cols, int rows)
{
	WINDOW *padcpu = *padcpuin;
	if (padcpu == NULL) {
		return;
	}
	int x = *xin;
	uibanner(cols, padcpu,"Linux and Processor Details");
	mvwprintw(padcpu, 1, 4, "Linux: %s", "??"); // proc[P_VERSION].line[0]);
	mvwprintw(padcpu, 2, 4, "Build: %s", "??"); // proc[P_VERSION].line[1]);
	mvwprintw(padcpu, 3, 4, "Release  : %s", "??"); // uts.release );
	mvwprintw(padcpu, 4, 4, "Version  : %s", "??"); // uts.version);
	mvwprintw(padcpu, 9, 4, "# of CPUs: %d", -1); // cpus);
	mvwprintw(padcpu, 10, 4,"Machine  : %s", "??"); // uts.machine);
	mvwprintw(padcpu, 11, 4,"Nodename : %s", "??"); // uts.nodename);
	mvwprintw(padcpu, 12, 4,"/etc/*ease[1]: %s", "??"); // easy[0]);
	mvwprintw(padcpu, 13, 4,"/etc/*ease[2]: %s", "??"); // easy[1]);
	mvwprintw(padcpu, 14, 4,"/etc/*ease[3]: %s", "??"); // easy[2]);
	mvwprintw(padcpu, 15, 4,"/etc/*ease[4]: %s", "??"); // easy[3]);
	mvwprintw(padcpu, 16, 4,"lsb_release: %s", "??"); // lsb_release[0]);
	mvwprintw(padcpu, 17, 4,"lsb_release: %s", "??"); // lsb_release[1]);
	mvwprintw(padcpu, 18, 4,"lsb_release: %s", "??"); // lsb_release[2]);
	mvwprintw(padcpu, 19, 4,"lsb_release: %s", "??"); // lsb_release[3]);
	uidisplay(&x, cols, 20, padcpu, rows);
	*xin = x;
}
