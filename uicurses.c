/**
 * uicurses.c -- Ncurses user interface elements of nmond
 *
 *
 * nmond -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
 *  https://github.com/stollcri/nmond
 * 
 * 
 * Copyright (c) 2015, Christopher Stoll
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of nmond nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "uicurses.h"

static inline void uibanner(WINDOW *win, int cols, char *string)
{
	mvwhline(win, 0, 0, ACS_HLINE, (cols - 2));
	wmove(win, 0, 0);
	wattron(win, A_STANDOUT);
	wprintw(win, " ");
	wprintw(win, string);
	wprintw(win, " ");
	wattroff(win, A_STANDOUT);
}


static inline void uidisplay(WINDOW *win, int *xin, int cols, int rows, int lines)
{
	int x = *xin;

	if((x + rows + 2) > lines) {
		pnoutrefresh(win, 0, 0, x, 1, (lines - 2), (cols - 2));
	} else {
		pnoutrefresh(win, 0, 0, x, 1, (x + rows + 1), (cols - 2));
	}

	x = x + rows;
	if((x + 4) > lines) {
		mvwprintw(stdscr, lines-1, 10, MSG_WRN_NOT_SHOWN);
	}

	*xin = x;
}

inline void uiheader(int *xin, int usecolor, int blinkon, char *hostname, double elapsed, time_t timer)
{
	int x = *xin;
	struct tm *tim = localtime(&timer);

	box(stdscr, 0, 0);
	mvprintw(x, 1, APPNAME);
	mvprintw(x, 9, VERSION);
	if(blinkon) {
		mvprintw(x, 15, "[H for help]");
	}
	mvprintw(x, 30, "%s", hostname);
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
	uibanner(win, cols, "HELP");
	mvwprintw(win,  1, 2, "[ a =                               ][ t = Top Processes                 ]");
	mvwprintw(win,  2, 2, "[ b = Black & White mode            ][                                   ]");
	mvwprintw(win,  3, 2, "[ c = CPU Load                      ][                                   ]");
	mvwprintw(win,  4, 2, "[ C = CPU Load, long-term           ][                                   ]");
	mvwprintw(win,  5, 2, "[ d =                               ][                                   ]");
	mvwprintw(win,  6, 2, "[ D =                               ][                                   ]");
	mvwprintw(win,  7, 2, "[ f =                               ][                                   ]");
	mvwprintw(win,  8, 2, "[ F =                               ][                                   ]");
	mvwprintw(win,  9, 2, "[ h = Help                          ][                                   ]");
	mvwprintw(win, 10, 2, "[ H = Help                          ][                                   ]");
	mvwprintw(win, 11, 2, "[ k = Kernel Information            ][                                   ]");
	mvwprintw(win, 12, 2, "[ m =                               ][                                   ]");
	mvwprintw(win, 13, 2, "[ M =                               ][                                   ]");
	mvwprintw(win, 14, 2, "[ n =                               ][                                   ]");
	mvwprintw(win, 15, 2, "[ N =                               ][                                   ]");
	mvwprintw(win, 16, 2, "[ q = Quit/Exit                     ][                                   ]");
<<<<<<< HEAD
	mvwprintw(win, 18, 2, "             Christopher Stoll, 2015 (%s)", APPURL);
=======
	mvwprintw(win, 18, 2, "        %s version %s build %s", APPNAME, VERSION, VERDATE);
	mvwprintw(win, 19, 2, "              Chrisotpher Stoll, 2015 (%s)", APPURL);
	
>>>>>>> f86a3d2c0ad93d69ebc80d9e5b69771d9e37ff48

	pnoutrefresh(win, 0, 0, x, 1, rows-2, cols-2);
	uidisplay(win, &x, cols, 20, rows);
	
	*xin = x;
}

void uisys(WINDOW **winin, int *xin, int cols, int rows, struct syskern kern)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}
	int x = *xin;
	uibanner(win, cols, "Kernel and Processor Details");
	mvwprintw(win, 1, 2, "%s", kern.version);
	mvwprintw(win, 2, 2, "Release  : %s", kern.osrelease);
	mvwprintw(win, 3, 2, "Version  : %s", kern.osversion);
	mvwprintw(win, 4, 2, "# of CPUs: %d", kern.corecount);
	mvwprintw(win, 5, 2, "Nodename : %s", kern.hostname);
	mvwprintw(win, 6, 2, "Domain   : %s", kern.domainname);
	mvwprintw(win, 7, 2, "Booted   : %s", kern.boottimestring);
	uidisplay(win, &x, cols, 8, rows);
	
	*xin = x;
}

void uiverbose(WINDOW **winin, int *xin, int cols, int rows)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}
	int x = *xin;

	uibanner(win, cols, "Verbose Mode");
	mvwprintw(win, 1, 0, " Code    Resource            Stats   Now\tWarn\tDanger ");

/*
					mvwprintw(win, 2, 0, "        -> CPU               %%busy %5.1f%%\t>80%%\t>90%%          ",cpu_busy);
					if(cpu_busy > 90.0){
						COLOUR wattrset(win, COLOR_PAIR(1));
						mvwprintw(win, 2, 0, " DANGER");
					}
					else if(cpu_busy > 80.0) {
						COLOUR wattrset(win, COLOR_PAIR(4));
						mvwprintw(win, 2, 0, "Warning");
					}
					else  {
						COLOUR wattrset(win, COLOR_PAIR(2));
						mvwprintw(win, 2, 0, "     OK");
					}
					COLOUR wattrset(win, COLOR_PAIR(0));

#define DKDELTA(member) ( (q->dk[i].member > p->dk[i].member) ? 0 : (p->dk[i].member - q->dk[i].member))

				top_disk_busy = 0.0;
				top_disk_name = "";
				for (i = 0, k = 0; i < disks; i++) {
					disk_busy = DKDELTA(dk_time) / elapsed;
					if( disk_busy > top_disk_busy) {
						top_disk_busy = disk_busy;
						top_disk_name = p->dk[i].dk_name;
					}
				}
				if(top_disk_busy > 80.0) {
					COLOUR wattrset(win, COLOR_PAIR(1));
					mvwprintw(win, 3, 0, " DANGER");
				} else if(top_disk_busy > 60.0) {
					COLOUR wattrset(win, COLOR_PAIR(4));
					mvwprintw(win, 3, 0, "Warning");
				} else  {
					COLOUR wattrset(win, COLOR_PAIR(2));
					mvwprintw(win, 3, 0, "     OK");
				}
				COLOUR wattrset(win, COLOR_PAIR(0));
				mvwprintw(win, 3, 8, "-> Top Disk %8s %%busy %5.1f%%\t>40%%\t>60%%          ",top_disk_name,top_disk_busy);
				move(x,0);


				y=x;
				x=1;
				DISPLAY(padverb,4);
				x=y;
*/
	uidisplay(win, &x, cols, 4, rows);

	*xin = x + 6;
}

void uicpudetail(WINDOW *win, int cpuno, int row, int usecolor, double user, double sys, double idle, double nice)
{
	if(cpuno == -1) {
		mvwprintw(win, row, 0, "Avg");
	} else {
		mvwprintw(win, row, 0, "%3d", cpuno);
	}
	mvwprintw(win, row,  4, "%4.2f ", user);
	mvwprintw(win, row, 10, "%4.2f ", sys);
	mvwprintw(win, row, 16, "%4.2f ", nice);
	mvwprintw(win, row, 22, "%4.2f ", idle);
	mvwprintw(win, row, 27, "|");

	wmove(win, row, 28);
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
				wattrset(win, COLOR_PAIR(10));
				wprintw(win, metermark);
			} else {
				wprintw(win, "U");
			}
			--userquant;
		} else {
			if(systquant) {
				if(usecolor) {
					wattrset(win, COLOR_PAIR(8));
					wprintw(win, metermark);
				} else {
					wprintw(win, "S");
				}
				--systquant;
			} else {
				if(nicequant) {
					if(usecolor) {
						wattrset(win, COLOR_PAIR(9));
						wprintw(win, metermark);
					} else {
						wprintw(win, "N");
					}
					--nicequant;
				} else {
					wattrset(win, COLOR_PAIR(0));
					wprintw(win, metermark);
				}
			}
		}
	}
	wattrset(win, COLOR_PAIR(0));
	mvwprintw(win, row, 77, "|");
}

void uicpu(WINDOW **winin, int *xin, int cols, int rows, int usecolor, struct sysres thisres, int show_raw)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}
	int x = *xin;

	uibanner(win, cols, "CPU Load");
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
	uidisplay(win, &x, cols, (cpuno + 3), rows);

	*xin = x;
}

void uicpulong(WINDOW **winin, int *xin, int cols, int rows, int *itterin, int usecolor, struct sysres thisres)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}
	int x = *xin;
	int itteration = *itterin;

	mvwprintw(win, 0, 0, " CPU +---Long-Term-----------------------------------------------------------+");
	if (usecolor){
		wattrset(win, COLOR_PAIR(4));
		mvwprintw(win, 0, 27, "User%%");
		wattrset(win, COLOR_PAIR(1));
		mvwprintw(win, 0, 35, "System%%");
		wattrset(win, COLOR_PAIR(2));
		mvwprintw(win, 0, 45, "Nice%%");
		wattrset(win, COLOR_PAIR(0));
	}
	mvwprintw(win, 1, 0, "100%%-|");
	mvwprintw(win, 2, 1, "95%%-|");
	mvwprintw(win, 3, 1, "90%%-|");
	mvwprintw(win, 4, 1, "85%%-|");
	mvwprintw(win, 5, 1, "80%%-|");
	mvwprintw(win, 6, 1, "75%%-|");
	mvwprintw(win, 7, 1, "70%%-|");
	mvwprintw(win, 8, 1, "65%%-|");
	mvwprintw(win, 9, 1, "60%%-|");
	mvwprintw(win, 10, 1, "55%%-|");
	mvwprintw(win, 11, 1, "50%%-|");
	mvwprintw(win, 12, 1, "45%%-|");
	mvwprintw(win, 13, 1, "40%%-|");
	mvwprintw(win, 14, 1, "35%%-|");
	mvwprintw(win, 15, 1, "30%%-|");
	mvwprintw(win, 16, 1, "25%%-|");
	mvwprintw(win, 17, 1, "20%%-|");
	mvwprintw(win, 18, 1, "15%%-|");
	mvwprintw(win, 19, 1, "10%%-|");
	mvwprintw(win, 20, 1, " 5%%-|");
	
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
				wattrset(win, COLOR_PAIR(10));
				wprintw(win, metermark);
			} else {
				wprintw(win, "U");
			}
			--userquant;
		} else {
			if(systquant) {
				if(usecolor) {
					wattrset(win, COLOR_PAIR(8));
					wprintw(win, metermark);
				} else {
					wprintw(win, "S");
				}
				--systquant;
			} else {
				if(nicequant) {
					if(usecolor) {
						wattrset(win, COLOR_PAIR(9));
						wprintw(win, metermark);
					} else {
						wprintw(win, "N");
					}
					--nicequant;
				} else {
					wattrset(win, COLOR_PAIR(0));
					wprintw(win, metermark);
				}
			}
		}

		wattrset(win, COLOR_PAIR(0));
		wmove(win, i, itteration+offset+1);
		wprintw(win, leadermark);
	}
	uidisplay(win, &x, cols, 21, rows);

	++itteration;
	if(itteration > graphcols) {
		itteration = 0;
	}

	*itterin = itteration;
	*xin = x;
}
