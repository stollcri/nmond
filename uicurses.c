/**
 * nmond_ui_curses.c -- Ncurses user interface elements of nmond
 *  Copyright (c) 2015 Christopher Stoll (https://github.com/stollcri)
 *   (for license, see included LICENSE file)
 */

#include "uicurses.h"

static inline void uibanner(int cols, WINDOW *pad, char *string)
{
	mvwhline(pad, 0, 0, ACS_HLINE, cols-2);
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
		pnoutrefresh(pad, 0, 0, x, 1, lines-2, cols-2);
	} else {
		pnoutrefresh(pad, 0, 0, x, 1, x+rows+1, cols-2);
	}

	x=x+(rows);
	if(x+4 > lines) {
		mvwprintw(stdscr, lines-1, 10, MSG_WRN_NOT_SHOWN);
	}

	*xin = x;
}

inline void uiheader(int *xin, int usecolor, int useblink, char *version, char *hostname, double elapsed, time_t timer)
{
	int x = *xin;
	struct tm *tim = localtime(&timer);

	box(stdscr, 0, 0);
	mvprintw(x, 2, "nmond");
	mvprintw(x, 8, "%s", version);
	if(useblink) {
		mvprintw(x, 14,"[H for help]");
	}
	mvprintw(x, 29, "%s", hostname);
	mvprintw(x, 52, "Refresh=%2.0fsecs ", elapsed);
	mvprintw(x, 70, "%02d:%02d.%02d", tim->tm_hour, tim->tm_min, tim->tm_sec);
	wnoutrefresh(stdscr);

	*xin = x + 1;
}

void uiwelcome(WINDOW **winin, int *xin, int cols, int rows, int usecolor, struct syshw hw)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}
	int x = *xin;

	if(usecolor) {
		wattrset(win, COLOR_PAIR(2));
	}
	mvwprintw(win, x+1, 1, "                                         __");
	mvwprintw(win, x+2, 1, "    ____   ____ ___   ____   ____   ____/ /");
	mvwprintw(win, x+3, 1, "   / __ \\ / __ `__ \\ / __ \\ / __ \\ / __  / ");
	mvwprintw(win, x+4, 1, "  / / / // / / / / // /_/ // / / // /_/ /  ");
	mvwprintw(win, x+5, 1, " /_/ /_//_/ /_/ /_/ \\____//_/ /_/ \\____/   ");
	mvwprintw(win, x+6, 1, "=======================================    ");
	if(usecolor) {
		wattrset(win, COLOR_PAIR(0));
	}
	mvwprintw(win, x+1, 45, "For help type H or ...");
	mvwprintw(win, x+2, 45, " nmond -?  - hint");
	mvwprintw(win, x+3, 45, " nmond -h  - full");
	mvwprintw(win, x+5, 45, "To start the same way every time");
	mvwprintw(win, x+6, 45, " set the NMOND shell variable");
	if(usecolor) {
		wattrset(win, COLOR_PAIR(1));
	}
	mvwprintw(win, x+8,  2, "%s", hw.model);
	mvwprintw(win, x+9,  2, "%s", hw.cpubrand);
	mvwprintw(win, x+10, 2, "CPU Count    = %2d   Physcal Cores = %d", hw.cpucount, hw.physicalcpucount);
	mvwprintw(win, x+11, 2, "Hyperthreads = %2d   Virtual CPUs  = %d", hw.hyperthreads, hw.logicalcpucount);
	if(usecolor) {
		wattrset(win, COLOR_PAIR(0));
	}
	mvwprintw(win, x+15, 3, "Use these keys to toggle statistics on/off:");
	mvwprintw(win, x+16, 3, "   c = CPU        l = CPU Long-term   - = Faster screen updates");
	mvwprintw(win, x+17, 3, "   m = Memory     j = Filesystems     + = Slower screen updates");
	mvwprintw(win, x+18, 3, "   d = Disks      n = Network         V = Virtual Memory");
	mvwprintw(win, x+19, 3, "   r = Resource   N = NFS             v = Verbose hints");
	mvwprintw(win, x+20, 3, "   k = kernel     t = Top-processes   . = only busy disks/procs");
	mvwprintw(win, x+21, 3, "   h = more options                   q = Quit");
	pnoutrefresh(win, 0, 0, x, 1, rows-2, cols-2);
	wnoutrefresh(stdscr);
	
	*xin = x + 22;
}

void uihelp(WINDOW **winin, int *xin, int cols, int rows)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}
	int x = *xin;
	uibanner(cols, win, "HELP");
	mvwprintw(win,  1, 5, "key  --- statistics which toggle on/off ---");
	mvwprintw(win,  2, 5, "h = This help information");
	mvwprintw(win,  3, 5, "r = RS6000/pSeries CPU/cache/OS/kernel/hostname details + LPAR");
	mvwprintw(win,  4, 5, "t = Top Process Stats 1=basic 3=CPU");
	mvwprintw(win,  5, 5, "    u = shows command arguments (hit twice to refresh)");
	mvwprintw(win,  6, 5, "c = CPU by processor             l = longer term CPU averages");
	mvwprintw(win,  7, 5, "m = Memory & Swap stats L=Huge   j = JFS Usage Stats");
	mvwprintw(win,  8, 5, "n = Network stats                N = NFS");
	mvwprintw(win,  9, 5, "d = Disk I/O Graphs D=Stats      o = Disks %%Busy Map");
	mvwprintw(win, 10, 5, "k = Kernel stats & loadavg       V = Virtual Memory");
	mvwprintw(win, 11, 5, "g = User Defined Disk Groups [start nmon with -g <filename>]");
	mvwprintw(win, 12, 5, "v = Verbose Simple Checks - OK/Warnings/Danger");
	mvwprintw(win, 13, 5, "b = black & white mode");
	mvwprintw(win, 14, 5, "--- controls ---");
	mvwprintw(win, 15, 5, "+ and - = double or half the screen refresh time");
	mvwprintw(win, 16, 5, "q = quit                     space = refresh screen now");
	mvwprintw(win, 17, 5, ". = Minimum Mode =display only busy disks and processes");
	mvwprintw(win, 18, 5, "0 = reset peak counts to zero (peak = \">\")");
	mvwprintw(win, 19, 5, "Chrisotpher Stoll, 2015 (https://github.com/stollcri)");
	pnoutrefresh(win, 0, 0, x, 1, rows-2, cols-2);
	uidisplay(&x, cols, 20, win, rows);
	
	*xin = x;
}

void uisys(WINDOW **winin, int *xin, int cols, int rows, struct syskern kern)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}
	int x = *xin;
	uibanner(cols, win, "Kernel and Processor Details");
	mvwprintw(win, 1, 2, "%s", kern.version);
	mvwprintw(win, 2, 2, "Release  : %s", kern.osrelease);
	mvwprintw(win, 3, 2, "Version  : %s", kern.osversion);
	mvwprintw(win, 4, 2, "# of CPUs: %d", kern.corecount);
	mvwprintw(win, 5, 2, "Nodename : %s", kern.hostname);
	mvwprintw(win, 6, 2, "Domain   : %s", kern.domainname);
	mvwprintw(win, 7, 2, "Booted   : %s", kern.boottimestring);
	uidisplay(&x, cols, 8, win, rows);
	
	*xin = x;
}

void uiverbose(WINDOW **winin, int *xin, int cols)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}
	int x = *xin;

	uibanner(cols, win, "Verbose Mode");
	mvwprintw(win, 1, 0, " Code    Resource            Stats   Now\tWarn\tDanger ");

	*xin = x + 6;
}

void uicpudetail(WINDOW *win, int cpuno, int row, int usecolor, double user, double sys, double idle, double nice)
{
	if(cpuno == -1) {
		mvwprintw(win,row, 0, "Avg");
	} else {
		mvwprintw(win,row, 0, "%3d", cpuno);
	}
	mvwprintw(win,row,  4, "%4.2f ", user);
	mvwprintw(win,row, 10, "%4.2f ", sys);
	mvwprintw(win,row, 16, "%4.2f ", nice);
	mvwprintw(win,row, 22, "%4.2f ", idle);
	mvwprintw(win,row, 27, "|");

	wmove(win,row, 28);
	char *metermark = (char*)malloc(sizeof(char));
	int userquant = (int)(round(user) / 2);
	int systquant = (int)(round(sys) / 2);
	int nicequant = (int)(round(nice) / 2);
	for(int i=28; i<77; ++i){
		if(((i + 3) % 5) == 0) {
			metermark = "|";
		} else {
			if(cpuno >= 0) {
				if(((cpuno + 1) % 2) == 0) {
					metermark = " ";
				} else {
					metermark = "-";
				}
			} else {
				metermark = "=";
			}
		}

		if(userquant) {
			if(usecolor) {
				wattrset(win,COLOR_PAIR(10));
				wprintw(win, metermark);
			} else {
				wprintw(win,"U");
			}
			--userquant;
		} else {
			if(systquant) {
				if(usecolor) {
					wattrset(win,COLOR_PAIR(8));
					wprintw(win, metermark);
				} else {
					wprintw(win,"S");
				}
				--systquant;
			} else {
				if(nicequant) {
					if(usecolor) {
						wattrset(win,COLOR_PAIR(9));
						wprintw(win, metermark);
					} else {
						wprintw(win,"N");
					}
					--nicequant;
				} else {
					wattrset(win,COLOR_PAIR(0));
					wprintw(win, metermark);
				}
			}
		}
	}
	wattrset(win,COLOR_PAIR(0));
	mvwprintw(win,row, 77, "|");
}

void uicpu(WINDOW **winin, int *xin, int cols, int rows, int usecolor, struct sysres thisres, int show_raw)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}
	int x = *xin;

	uibanner(cols, win, "CPU Load");
	mvwprintw(win, 1, 0, "CPU");
	if(usecolor) {
		wattrset(win, COLOR_PAIR(COLOR_BLUE));
		mvwprintw(win, 1, 4, "User%%");
		wattrset(win, COLOR_PAIR(COLOR_RED));
		mvwprintw(win, 1, 10, "Sys %%");
		wattrset(win, COLOR_PAIR(COLOR_GREEN));
		mvwprintw(win, 1, 16, "Nice%%");
		wattrset(win, COLOR_PAIR(COLOR_WHITE));
		mvwprintw(win, 1, 22, "Idle");
	} else {
		mvwprintw(win, 1, 4, "User%%");
		mvwprintw(win, 1, 10, "Sys %%");
		mvwprintw(win, 1, 16, "Wait%%");
		mvwprintw(win, 1, 22, "Idle");
	}
	mvwprintw(win, 1, 27, "|0   |  20|    |  40|    |  60|    |  80|    | 100|");

	int cpuno = 0;
	for (cpuno = 0; cpuno < thisres.cpucount; ++cpuno) {
	 	mvwprintw(win, (2 + cpuno), 77, "|");
		uicpudetail(win, cpuno, (cpuno + 2), usecolor,
			thisres.cpus[cpuno].percentuser, 
			thisres.cpus[cpuno].percentsys,
			thisres.cpus[cpuno].percentidle,
			thisres.cpus[cpuno].percentnice);
	}

	if (thisres.cpucount > 1) {
		uicpudetail(win, -1, (cpuno + 2), usecolor,
			thisres.avgpercentuser, 
			thisres.avgpercentsys, 
			thisres.avgpercentidle, 
			thisres.avgpercentnice);
	}
	uidisplay(&x, cols, (cpuno + 3), win, rows);

	*xin = x;
}

#define COLOUR if(colour)
void uicpulong(WINDOW **winin, int *xin, int cols, int rows, int *itterin, int usecolor, struct sysres thisres)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}
	int x = *xin;
	int itteration = *itterin;

	mvwprintw(win,0, 0, " CPU +---Long-Term-----------------------------------------------------------+");
	if (usecolor){
		wattrset(win, COLOR_PAIR(4));
		mvwprintw(win,0, 27, "User%%");
		wattrset(win, COLOR_PAIR(1));
		mvwprintw(win,0, 35, "System%%");
		wattrset(win, COLOR_PAIR(2));
		mvwprintw(win,0, 45, "Nice%%");
		wattrset(win, COLOR_PAIR(0));
	}
	mvwprintw(win,1, 0,"100%%-|");
	mvwprintw(win,2, 1, "95%%-|");
	mvwprintw(win,3, 1, "90%%-|");
	mvwprintw(win,4, 1, "85%%-|");
	mvwprintw(win,5, 1, "80%%-|");
	mvwprintw(win,6, 1, "75%%-|");
	mvwprintw(win,7, 1, "70%%-|");
	mvwprintw(win,8, 1, "65%%-|");
	mvwprintw(win,9, 1, "60%%-|");
	mvwprintw(win,10, 1, "55%%-|");
	mvwprintw(win,11, 1, "50%%-|");
	mvwprintw(win,12, 1, "45%%-|");
	mvwprintw(win,13, 1, "40%%-|");
	mvwprintw(win,14, 1, "35%%-|");
	mvwprintw(win,15, 1, "30%%-|");
	mvwprintw(win,16, 1, "25%%-|");
	mvwprintw(win,17, 1, "20%%-|");
	mvwprintw(win,18, 1,"15%%-|");
	mvwprintw(win,19, 1,"10%%-|");
	mvwprintw(win,20, 1," 5%%-|");
	
	int graphcols = 70;
	int graphrows = 20;
	int offset = 6;

	char *metermark = (char*)malloc(sizeof(char));
	char *leadermark = (char*)malloc(sizeof(char));

	int userquant = (int)(round(thisres.avgpercentuser) / 5);
	int systquant = (int)(round(thisres.avgpercentsys) / 5);
	int nicequant = (int)(round(thisres.avgpercentnice) / 5);

	for (int i = graphrows; i > 0; --i) {
		wmove(win, i, itteration+offset);
		
		if((i > 1) && (((i - 1) % 4) == 0)) {
			metermark = "-";
			leadermark = "+";
		} else {
			metermark = " ";
			leadermark = "|";
		}

		if(userquant) {
			if(usecolor) {
				wattrset(win,COLOR_PAIR(10));
				wprintw(win, metermark);
			} else {
				wprintw(win,"U");
			}
			--userquant;
		} else {
			if(systquant) {
				if(usecolor) {
					wattrset(win,COLOR_PAIR(8));
					wprintw(win, metermark);
				} else {
					wprintw(win,"S");
				}
				--systquant;
			} else {
				if(nicequant) {
					if(usecolor) {
						wattrset(win,COLOR_PAIR(9));
						wprintw(win, metermark);
					} else {
						wprintw(win,"N");
					}
					--nicequant;
				} else {
					wattrset(win,COLOR_PAIR(0));
					wprintw(win, metermark);
				}
			}
		}

		wattrset(win,COLOR_PAIR(0));
		wmove(win, i, itteration+offset+1);
		wprintw(win, leadermark);
	}
	uidisplay(&x, cols, 21, win, rows);

	++itteration;
	if(itteration > graphcols) {
		itteration = 0;
	}

	*itterin = itteration;
	*xin = x;
}
