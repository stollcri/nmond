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
#include <sys/sysctl.h>
#include <math.h>
#include <string.h>
#include "uibytesize.h"

static inline void uibanner(WINDOW *win, int cols, char *string)
{
	wattrset(win, COLOR_PAIR(12));
	wattron(win, A_STANDOUT);
	// mvwhline(win, 0, 0, ACS_HLINE, (cols - 2));
	mvwhline(win, 0, 0, ' ', (cols - 2));
	wattroff(win, A_STANDOUT);
	wattrset(win, COLOR_PAIR(0));

	wmove(win, 0, 0);

	wattron(win, A_STANDOUT);
	wprintw(win, " ");
	wprintw(win, string);
	wprintw(win, " ");
	wattroff(win, A_STANDOUT);
}


static inline void uidisplay(WINDOW *win, int *currow, int cols, int lines, int rows)
{
	int rowstart = *currow + 1;
	int rowend = *currow + rows;
	int colstart = BORDER_WIDTH / 2;
	int colend = cols - BORDER_WIDTH;

	if(*currow < 0) {
		rowstart = 1;
	}

	if((rowend + BORDER_WIDTH) > lines) {
		rowend = lines - BORDER_WIDTH;
	}

	pnoutrefresh(win, 0, 0, rowstart, colstart, rowend, colend);

	*currow = *currow + rows;
	if((*currow + 1) > lines) {
		mvwprintw(stdscr, lines-1, 10, MSG_WRN_NOT_SHOWN);
	}
}

void uiheader(WINDOW **win, int currow, int usecolor, int blinkon, char *hostname, char *message, double elapsed, time_t timer)
{
	struct tm *tim = localtime(&timer);

	box(stdscr, 0, 0);
	mvwprintw(*win, currow, 3, APPNAME);
	mvwprintw(*win, currow, 11, "%s", hostname);
	if(blinkon) {
		if(strlen(message)) {
			attrset(COLOR_PAIR(8));
			// TODO: should limit to 26 characters
			mvwprintw(*win, currow, 35, "%s", message);
			attrset(COLOR_PAIR(0));
		}
	}
	mvwprintw(*win, currow, 64, "%1.0fs", elapsed);
	mvwprintw(*win, currow, 70, "%02d:%02d.%02d", tim->tm_hour, tim->tm_min, tim->tm_sec);

	wnoutrefresh(*win);
}

void uiwelcome(WINDOW **win, int winheight, int *currow, int cols, int lines, int usecolor, struct syshw hw)
{
	if (*win == NULL) {
		return;
	}
	wclear(*win);

	if(usecolor) {
		wattrset(*win, COLOR_PAIR(2));
	}
	mvwprintw(*win, *currow+1, 18, "                                         __");
	mvwprintw(*win, *currow+2, 18, "    ____   ____ ___   ____   ____   ____/ /");
	mvwprintw(*win, *currow+3, 18, "   / __ \\ / __ `__ \\ / __ \\ / __ \\ / __  / ");
	mvwprintw(*win, *currow+4, 18, "  / / / // / / / / // /_/ // / / // /_/ /  ");
	mvwprintw(*win, *currow+5, 18, " /_/ /_//_/ /_/ /_/ \\____//_/ /_/ \\____/   ");
	mvwprintw(*win, *currow+6, 18, "=======================================    ");
	// mvwprintw(*win, *currow+6, 18, "================================/nmond/    ");
	// mvwprintw(*win, *currow+6, 18, "-------------------------------[nmond]-    ");

	if(usecolor) {
		wattrset(*win, COLOR_PAIR(0));
	}

	if(usecolor) {
		wattrset(*win, COLOR_PAIR(1));
	}
	mvwprintw(*win, *currow+9,  0, "  %s", hw.model);
	mvwprintw(*win, *currow+10, 0, "  %s", hw.cpubrand);
	mvwprintw(*win, *currow+11, 0, "  CPU Count    = %2d   Physcal Cores = %d", hw.cpucount, hw.physicalcpucount);
	mvwprintw(*win, *currow+12, 0, "  Hyperthreads = %2d   Virtual CPUs  = %d", hw.hyperthreads, hw.logicalcpucount);
	if(usecolor) {
		wattrset(*win, COLOR_PAIR(0));
	}
	mvwprintw(*win, *currow+14, 0, "  Use these keys to toggle statistics on/off:");
	mvwprintw(*win, *currow+15, 0, "    c = CPU               n = Network Usage       - = Reduce refresh delay  ");
	mvwprintw(*win, *currow+16, 0, "    C = CPU, Long-term    r = Top-procs,mem-sort  + = Increase refresh delay");
	mvwprintw(*win, *currow+17, 0, "    d = Disk Usage        R = Top-procs/cmnd,mem  ? = Help                  ");
	mvwprintw(*win, *currow+18, 0, "    i = About this Mac    t = Top-processes                                 ");
	mvwprintw(*win, *currow+19, 0, "    m = Memory Usage      T = Top-procs/command   q = Quit                  ");
	mvwprintw(*win, *currow+21, 0, " To start the same way every time set an NMOND variable: 'export NMOND=cdnT'");

	uidisplay(*win, currow, cols, lines, winheight);
}

void uihelp(WINDOW **win, int winheight, int *currow, int cols, int lines)
{
	if (*win == NULL) {
		return;
	}
	wclear(*win);

	int currowsave = *currow;
	if(*currow > 0) {
		*currow = 0;
	}

	mvwprintw(*win, *currow+1,  0, "  [ a =                               ][ N =  Network Usage, long-term     ]");
	mvwprintw(*win, *currow+2,  0, "  [ b = Black & White mode            ][ o =                               ]");
	mvwprintw(*win, *currow+3,  0, "  [ c = CPU Load                      ][ r = Top Processes, order by mem   ]");
	mvwprintw(*win, *currow+4,  0, "  [ C = CPU Load, long-term           ][ R = Top Processes, command by mem ]");
	mvwprintw(*win, *currow+5,  0, "  [ d =                               ][ t = Top Processes, order by proc  ]");
	mvwprintw(*win, *currow+6,  0, "  [ D =                               ][ T = Top Processes, command by prc ]");
	mvwprintw(*win, *currow+7,  0, "  [ f =                               ][ v =                               ]");
	mvwprintw(*win, *currow+8,  0, "  [ F =                               ][ w =                               ]");
	mvwprintw(*win, *currow+9,  0, "  [ h = Help                          ][                                   ]");
	mvwprintw(*win, *currow+10, 0, "  [ H = Help                          ][                                   ]");
	mvwprintw(*win, *currow+11, 0, "  [ i = About This Mac                ][ - = Reduce refresh delay (half)   ]");
	mvwprintw(*win, *currow+12, 0, "  [ I =                               ][ + = Increase refresh delay (2x)   ]");
	mvwprintw(*win, *currow+13, 0, "  [ k =                               ][                                   ]");
	mvwprintw(*win, *currow+14, 0, "  [ m = Memory Usage                  ][ ? = Help                          ]");
	mvwprintw(*win, *currow+15, 0, "  [ M =                               ][                                   ]");
	mvwprintw(*win, *currow+16, 0, "  [ n = Network Usage                 ][ q = Quit/Exit                     ]");
	mvwprintw(*win, *currow+17, 0, "                                                                            ");
	mvwprintw(*win, *currow+18, 0, "          %s version %s build %s", APPNAME, VERSION, VERDATE);
	mvwprintw(*win, *currow+19, 0, "             Christopher Stoll, 2015-2020 (%s)", APPURL);
	uibanner(*win, cols, "HELP");
	*currow = currowsave;
	uidisplay(*win, currow, cols, lines, winheight);
}

static void uicpudetail(WINDOW *win, int cpuno, int row, int usecolor, double user, double sys, double idle, double nice)
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
	if(((cpuno + 1) % 2) || (cpuno < 0)) {
		mvwaddch(win, row, 27, ACS_VLINE);
	} else {
		mvwaddch(win, row, 27, ACS_LTEE);
	}

	wmove(win, row, 28);

	chtype metermark;

	int userquant = (int)(round(user) / 2);
	int systquant = (int)(round(sys) / 2);
	int nicequant = (int)(round(nice) / 2);
	for(int i=28; i<77; ++i){
		if(((i + 3) % 5) == 0) {
			if(((cpuno + 1) % 2) || (cpuno < 0)) {
				metermark = ACS_VLINE;
			} else {
				metermark = ACS_PLUS;
			}
		} else {
			if(cpuno >= 0) {
				if((cpuno + 1) % 2) {
					metermark = ' ';
				} else {
					metermark = ACS_HLINE;
				}
			} else {
				metermark = ' ';
			}
		}

		if(userquant) {
			if(usecolor) {
				wattrset(win, COLOR_PAIR(10));
				waddch(win, metermark);
			} else {
				wattron(win, A_STANDOUT);
				waddch(win, ACS_CKBOARD);
				wattroff(win, A_STANDOUT);
			}
			--userquant;
		} else {
			if(systquant) {
				if(usecolor) {
					wattrset(win, COLOR_PAIR(8));
					waddch(win, metermark);
				} else {
					waddch(win, ACS_CKBOARD);
				}
				--systquant;
			} else {
				if(nicequant) {
					if(usecolor) {
						wattrset(win, COLOR_PAIR(9));
						waddch(win, metermark);
					} else {
						waddch(win, ACS_DIAMOND);
					}
					--nicequant;
				} else {
					wattrset(win, COLOR_PAIR(0));
					waddch(win, metermark);
				}
			}
		}
	}
	wattrset(win, COLOR_PAIR(0));
	if(((cpuno + 1) % 2) || (cpuno < 0)) {
		mvwaddch(win, row, 77, ACS_VLINE);
	} else {
		mvwaddch(win, row, 77, ACS_RTEE);
	}
}

void uicpu(WINDOW **win, int winheight, int *currow, int cols, int lines, int usecolor, struct sysres thisres, int show_raw)
{
	if (*win == NULL) {
		return;
	}
	wclear(*win);

	int currowsave = *currow;
	if(*currow > 0) {
		*currow = 0;
	}

	mvwprintw(*win, *currow+1, 0, "CPU");
	if(usecolor) {
		wattrset(*win, COLOR_PAIR(4));
		mvwprintw(*win, *currow+1, 4, "User%%");
		wattrset(*win, COLOR_PAIR(1));
		mvwprintw(*win, *currow+1, 10, "Sys %%");
		wattrset(*win, COLOR_PAIR(2));
		mvwprintw(*win, *currow+1, 16, "Nice%%");
		wattrset(*win, COLOR_PAIR(0));
		mvwprintw(*win, *currow+1, 22, "Idle");
	} else {
		mvwprintw(*win, *currow+1, 4, "User%%");
		mvwprintw(*win, *currow+1, 10, "Sys %%");
		mvwprintw(*win, *currow+1, 16, "Wait%%");
		mvwprintw(*win, *currow+1, 22, "Idle");
	}
	mvwhline(*win, *currow+1, 27, ACS_HLINE, 50);
	mvwaddch(*win, *currow+1, 27, ACS_ULCORNER);
	mvwaddch(*win, *currow+1, 32, ACS_TTEE);
	mvwprintw(*win, *currow+1, 35, "20");
	mvwaddch(*win, *currow+1, 37, ACS_PLUS);
	mvwaddch(*win, *currow+1, 42, ACS_TTEE);
	mvwprintw(*win, *currow+1, 45, "40");
	mvwaddch(*win, *currow+1, 47, ACS_PLUS);
	mvwaddch(*win, *currow+1, 52, ACS_TTEE);
	mvwprintw(*win, *currow+1, 55, "60");
	mvwaddch(*win, *currow+1, 57, ACS_PLUS);
	mvwaddch(*win, *currow+1, 62, ACS_TTEE);
	mvwprintw(*win, *currow+1, 65, "80");
	mvwaddch(*win, *currow+1, 67, ACS_PLUS);
	mvwaddch(*win, *currow+1, 72, ACS_TTEE);
	mvwaddch(*win, *currow+1, 77, ACS_URCORNER);

	int cpuno = 0;
	for (cpuno = 0; cpuno < thisres.cpucount; ++cpuno) {
	 	mvwaddch(*win, (*currow+2 + cpuno), 77, ACS_VLINE);
		uicpudetail(*win, cpuno, (*currow+2 + cpuno), usecolor,
			thisres.cpus[cpuno].percentuser,
			thisres.cpus[cpuno].percentsys,
			thisres.cpus[cpuno].percentidle,
			thisres.cpus[cpuno].percentnice);
	}

	if (thisres.cpucount > 1) {
		uicpudetail(*win, -1, (*currow+2 + cpuno), usecolor,
			thisres.avgpercentuser,
			thisres.avgpercentsys,
			thisres.avgpercentidle,
			thisres.avgpercentnice);
	}

	uibanner(*win, cols, "CPU Load");
	*currow = currowsave;
	uidisplay(*win, currow, cols, lines, winheight);
}

void uicpulong(WINDOW **win, int winheight, int *currow, int cols, int lines, int itterin, int usecolor, int *longvals, int valcount)
{
	if (*win == NULL) {
		return;
	}
	wclear(*win);

	int currowsave = *currow;
	if(*currow > 0) {
		*currow = 0;
	}

	int tempvalue = 0;
	int graphlines = 10 + *currow;
	int offset = 6;

	int userquant = 0;
	int systquant = 0;
	int nicequant = 0;

	mvwprintw(*win, *currow+1,  0, "100%%-|");
	mvwprintw(*win, *currow+2,  0, " 90%%-|");
	mvwprintw(*win, *currow+3,  0, " 80%%-|");
	mvwprintw(*win, *currow+4,  0, " 70%%-|");
	mvwprintw(*win, *currow+5,  0, " 60%%-|");
	mvwprintw(*win, *currow+6,  0, " 50%%-|");
	mvwprintw(*win, *currow+7,  0, " 40%%-|");
	mvwprintw(*win, *currow+8,  0, " 30%%-|");
	mvwprintw(*win, *currow+9,  0, " 20%%-|");
	mvwprintw(*win, *currow+10, 0, " 10%%-|");
	mvwvline(*win, 1, offset-1, ACS_VLINE, graphlines);

	for (int j = 0; j < valcount; ++j) {
		tempvalue = j * 3;
		userquant = longvals[tempvalue];
		systquant = longvals[tempvalue+1];
		nicequant = longvals[tempvalue+2];

		if(j != itterin) {
			for (int i = graphlines; i > 0; --i) {
				wmove(*win, i, j+offset);

				if(userquant) {
					if(usecolor) {
						wattrset(*win, COLOR_PAIR(10));
						waddch(*win, ACS_VLINE);
					} else {
						wattron(*win, A_STANDOUT);
						waddch(*win, ACS_CKBOARD);
						wattroff(*win, A_STANDOUT);
					}
					--userquant;
				} else {
					if(systquant) {
						if(usecolor) {
							wattrset(*win, COLOR_PAIR(8));
							waddch(*win, ACS_VLINE);
						} else {
							waddch(*win, ACS_CKBOARD);
						}
						--systquant;
					} else {
						if(nicequant) {
							if(usecolor) {
								wattrset(*win, COLOR_PAIR(9));
								waddch(*win, ACS_VLINE);
							} else {
								waddch(*win, ACS_DIAMOND);
							}
							--nicequant;
						} else {
							wattrset(*win, COLOR_PAIR(0));
							waddch(*win, ' ');
						}
					}
				}
				wattrset(*win, COLOR_PAIR(0));
				wmove(*win, i, itterin+offset+1);
			}
		} else {
			mvwvline(*win, 1, j+offset, ACS_VLINE, graphlines);
		}
	}

	uibanner(*win, cols, "CPU Load");
	*currow = currowsave;
	uidisplay(*win, currow, cols, lines, winheight);
}

static void uidiskdetail(WINDOW *win, int currow, int usecolor, unsigned long diskr, unsigned long diskw, double unitdivisor, char *units, int scale)
{
	char *bytestring = NULL;

	bytestring = uireadablebyteslong(diskr);
	mvwprintw(win, currow-1, 2, "Reads:  %9.9s", bytestring);
	free(bytestring);
	bytestring = NULL;
	bytestring = uireadablebyteslong(diskw);
	mvwprintw(win, currow, 2, "Writes: %9.9s", bytestring);
	free(bytestring);
	bytestring = NULL;

	if(usecolor) {
		wattrset(win, COLOR_PAIR(4));
		bytestring = uireadablebyteslong(diskr);
		mvwprintw(win, currow-1, 10, "%9.9s", bytestring);
		free(bytestring);
		bytestring = NULL;
		wattrset(win, COLOR_PAIR(1));
        bytestring = uireadablebyteslong(diskw);
		mvwprintw(win, currow, 10, "%9.9s", bytestring);
		free(bytestring);
		bytestring = NULL;
		wattrset(win, COLOR_PAIR(0));
	}

	chtype metermark;
	int readquant;
	int writequant;

	if(!scale) {
		if((diskr > 0) || (diskw > 0)) {
			double disktotal = (double)(diskr + diskw);
			int tmpquant = (int)floor(log10(disktotal) * 4.9);
			// TODO: this ratio cannot be right for logrithmic output
			readquant = (int)(tmpquant * (diskr / (disktotal)));
			writequant = (int)(tmpquant * (diskw / (disktotal)));
		} else {
			readquant = 0;
			writequant = 0;
		}
		mvwprintw(win, currow-1, 22, "  LOG");
		mvwprintw(win, currow, 22, "SCALE");
	} else {
		readquant = (int)(floor(diskr) / (unitdivisor * 2 * scale));
		writequant = (int)(floor(diskw) / (unitdivisor * 2 * scale));
		mvwprintw(win, currow, 24, "%2.2s", units);
	}

	mvwaddch(win, currow, 27, ACS_VLINE);
	wmove(win, currow, 28);

	for(int i = 28; i < 77; ++i){
		if(((i + 3) % 5) == 0) {
			metermark = ACS_VLINE;
		} else {
			metermark = ' ';
		}

		if(readquant) {
			if(usecolor) {
				wattrset(win, COLOR_PAIR(10));
				waddch(win, metermark);
			} else {
				wattron(win, A_STANDOUT);
				waddch(win, ACS_CKBOARD);
				wattron(win, A_STANDOUT);
			}
			--readquant;
		} else {
			if(writequant) {
				if(usecolor) {
					wattrset(win, COLOR_PAIR(8));
					waddch(win, metermark);
				} else {
					waddch(win, ACS_CKBOARD);
				}
				--writequant;
			} else {
				wattrset(win, COLOR_PAIR(0));
				waddch(win, metermark);
			}
		}
	}
	wattrset(win, COLOR_PAIR(0));
	mvwaddch(win, currow, 77, ACS_VLINE);
}

extern void uidisks(WINDOW **win, int winheight, int *currow, int cols, int lines, int usecolor, unsigned int diskr, unsigned int diskw)
{
	if (*win == NULL) {
		return;
	}
	wclear(*win);

	int currowsave = *currow;
	if(*currow > 0) {
		*currow = 0;
	}

	unsigned int disktotal = diskr + diskw;

	if(DISK_METER_MODE == DISK_METER_LOG) {
		mvwaddch(*win, *currow+1, 27, ACS_VLINE);
 		wprintw(*win, " 10B");
 		waddch(*win, ACS_VLINE);
 		wprintw(*win, "100B");
 		waddch(*win, ACS_VLINE);
 		wprintw(*win, "  1K");
 		waddch(*win, ACS_VLINE);
 		wprintw(*win, " 10K");
 		waddch(*win, ACS_VLINE);
 		wprintw(*win, "100K");
 		waddch(*win, ACS_VLINE);
 		wprintw(*win, "  1M");
 		waddch(*win, ACS_VLINE);
 		wprintw(*win, " 10M");
 		waddch(*win, ACS_VLINE);
 		wprintw(*win, "100M");
 		waddch(*win, ACS_VLINE);
 		wprintw(*win, "  1G");
 		waddch(*win, ACS_VLINE);
 		wprintw(*win, " 10G");
 		waddch(*win, ACS_VLINE);
 		uidiskdetail(*win, *currow+2, usecolor, diskr, diskw, 0, "", 0);

	} else if(DISK_METER_MODE == DISK_METER_SCALE) {
	 	if(disktotal <= (BYTES_IN_KB * 100)) {
	 		mvwprintw(*win, *currow+1, 27, "|0   |  20|    |  40|    |  60|    |  80|    | 100|");
	 		mvwaddch(*win, *currow+2, 77, ACS_VLINE);
	 		uidiskdetail(*win, *currow+2, usecolor, diskr, diskw, BYTES_IN_KB, "KB", 1);

	 	} else if(disktotal <= (BYTES_IN_KB * 1000)) {
	 		mvwprintw(*win, *currow+1, 27, "|0   | 200|    | 400|    | 600|    | 800|    |1000|");
	 		mvwaddch(*win, *currow+2, 77, ACS_VLINE);
	 		uidiskdetail(*win, *currow+2, usecolor, diskr, diskw, BYTES_IN_KB, "KB", 10);

	 	} else {
	 		if(disktotal <= (BYTES_IN_MB * 100)) {
	 			mvwprintw(*win, *currow+1, 27, "|0   |  20|    |  40|    |  60|    |  80|    | 100|");
		 		mvwaddch(*win, *currow+2, 77, ACS_VLINE);
		 		uidiskdetail(*win, *currow+2, usecolor, diskr, diskw, BYTES_IN_MB, "MB", 1);

	 		} else if(disktotal <= (BYTES_IN_MB * 1000)) {
		 		mvwprintw(*win, *currow+1, 27, "|0   | 200|    | 400|    | 600|    | 800|    |1000|");
		 		mvwaddch(*win, *currow+2, 77, ACS_VLINE);
		 		uidiskdetail(*win, *currow+2, usecolor, diskr, diskw, BYTES_IN_MB, "MB", 10);

		 	} else {
		 		if(disktotal <= (BYTES_IN_GB * 100)) {
		 			mvwprintw(*win, *currow+1, 27, "|0   |  20|    |  40|    |  60|    |  80|    | 100|");
			 		mvwaddch(*win, *currow+2, 77, ACS_VLINE);
			 		uidiskdetail(*win, *currow+2, usecolor, diskr, diskw, BYTES_IN_GB, "GB", 1);

			 	} else {
			 		mvwprintw(*win, *currow+1, 27, "|0   | 200|    | 400|    | 600|    | 800|    |1000|");
			 		mvwaddch(*win, *currow+2, 77, ACS_VLINE);
			 		uidiskdetail(*win, *currow+2, usecolor, diskr, diskw, BYTES_IN_GB, "GB", 10);
			 	}
			}
	 	}
	} else {
		mvwprintw(*win, *currow+1, 27, "|0   |  20|    |  40|    |  60|    |  80|    | 100|");
 		mvwaddch(*win, *currow+2, 77, ACS_VLINE);
 		uidiskdetail(*win, *currow+2, usecolor, diskr, diskw, BYTES_IN_MB, "MB", 1);
	}

	uibanner(*win, cols, "Disk Usage");
	*currow = currowsave;
	uidisplay(*win, currow, cols, lines, winheight);
}

void uidisklong(WINDOW **win, int winheight, int *currow, int cols, int lines, int itterin, int usecolor, unsigned int *longvals, int valcount)
{
	if (*win == NULL) {
		return;
	}
	// wclear(*win);

	int currowsave = *currow;
	if(*currow > 0) {
		*currow = 0;
	}

	int tempvalue = 0;
	int graphlines = 10 + *currow;
	int offset = 6;

	unsigned int diskr = 0;
	unsigned int diskw = 0;
	double disktotal = 0;
	int tmpquant = 0;
	int readquant = 0;
	int writequant = 0;

	mvwprintw(*win, *currow+1,  0, " 10G-|");
	mvwprintw(*win, *currow+2,  0, "  1G-|");
	mvwprintw(*win, *currow+3,  0, "100M-|");
	mvwprintw(*win, *currow+4,  0, " 10M-|");
	mvwprintw(*win, *currow+5,  0, "  1M-|");
	mvwprintw(*win, *currow+6,  0, "100K-|");
	mvwprintw(*win, *currow+7,  0, " 10K-|");
	mvwprintw(*win, *currow+8,  0, "  1K-|");
	mvwprintw(*win, *currow+9,  0, "100B-|");
	mvwprintw(*win, *currow+10, 0, " 10B-|");
	mvwvline(*win, 1, offset-1, ACS_VLINE, graphlines);

	for (int j = 0; j < valcount; ++j) {
		tempvalue = j * 2;
		diskr = longvals[tempvalue];
		diskw = longvals[tempvalue+1];
		readquant = 0;
		writequant = 0;

		disktotal = (double)(diskr + diskw);
		if(disktotal) {
			tmpquant = (int)floor(log10(disktotal));
			// TODO: this ratio cannot be right for logrithmic output
			readquant = (int)(tmpquant * (diskr / (disktotal))) - 0;
			writequant = (int)(tmpquant * (diskw / (disktotal))) - 0;
		}

		if(j != itterin) {
			for (int i = graphlines; i > 0; --i) {
				wmove(*win, i, j+offset);

				if(readquant) {
					if(usecolor) {
						wattrset(*win, COLOR_PAIR(10));
						waddch(*win, ACS_VLINE);
					} else {
						wattron(*win, A_STANDOUT);
						waddch(*win, ACS_CKBOARD);
						wattroff(*win, A_STANDOUT);
					}
					--readquant;
				} else {
					if(writequant) {
						if(usecolor) {
							wattrset(*win, COLOR_PAIR(8));
							waddch(*win, ACS_VLINE);
						} else {
							waddch(*win, ACS_CKBOARD);
						}
						--writequant;
					} else {
						wattrset(*win, COLOR_PAIR(0));
						waddch(*win, ' ');
					}
				}
				wattrset(*win, COLOR_PAIR(0));
				wmove(*win, i, itterin+offset+1);
			}
		} else {
			mvwvline(*win, 1, j+offset, ACS_VLINE, graphlines);
		}
	}

	uibanner(*win, cols, "Disk Usage");
	*currow = currowsave;
	uidisplay(*win, currow, cols, lines, winheight);
}

extern void uidiskgroup(WINDOW **winin, int winheight, int *currow, int cols, int lines)
{
	return;
}

extern void uidiskmap(WINDOW **winin, int winheight, int *currow, int cols, int lines)
{
	return;
}

extern void uifilesys(WINDOW **winin, int winheight, int *currow, int cols, int lines)
{
	return;
}

extern void uikernel(WINDOW **winin, int winheight, int *currow, int cols, int lines)
{
	return;
}

static void uimemdetail(WINDOW *win, int currow, int usecolor, unsigned long long used, unsigned long long total, double percent, unsigned long long swap)
{
	char *bytestring_swap = uireadablebyteslonglong(swap);
	mvwprintw(win, currow-1, 2, "%8s swap", bytestring_swap);
	free(bytestring_swap);
	bytestring_swap = NULL;
	mvwprintw(win, currow-1, 24, "LOG");

	char *bytestring_used = uireadablebyteslonglong(used);
	char *bytestring_total = uireadablebyteslonglong(total);
	mvwprintw(win, currow, 2, "%8s/%-8s %5.2f%%", bytestring_used, bytestring_total, percent);
	free(bytestring_used);
	bytestring_used = NULL;
	free(bytestring_total);
	bytestring_total = NULL;

	if(usecolor) {
		wattrset(win, COLOR_PAIR(4));
		mvwprintw(win, currow, 20, "%5.2f%%", percent);
		wattrset(win, COLOR_PAIR(0));
	}

	mvwaddch(win, currow, 27, ACS_VLINE);
	wmove(win, currow, 28);

	chtype metermark;
	int usedquant = (int)(((log2(used) - 27.0) * 5) + 5) - 1;

	for(int i=28; i<77; ++i){
		if(((i + 3) % 5) == 0) {
			metermark = ACS_VLINE;
		} else {
			metermark = ' ';
		}

		if(usedquant >= 0) {
			if(usecolor) {
				wattrset(win, COLOR_PAIR(10));
				waddch(win, metermark);
			} else {
				waddch(win, ACS_CKBOARD);
			}
			--usedquant;
		} else {
			wattrset(win, COLOR_PAIR(0));
			waddch(win, metermark);
		}
	}
	wattrset(win, COLOR_PAIR(0));
	mvwaddch(win, currow, 77, ACS_VLINE);
}

extern void uimemory(WINDOW **win, int winheight, int *currow, int cols, int lines, int usecolor, unsigned long long memused, unsigned long long memtotal, unsigned long long swap)
{
	if (*win == NULL) {
		return;
	}
	wclear(*win);

	int currowsave = *currow;
	if(*currow > 0) {
		*currow = 0;
	}

	double percent = (double)((memused / 100.0) / (memtotal / 100.0) * 100);

	mvwaddch(*win, *currow+1, 27, ACS_VLINE);
	wprintw(*win, "128M");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "250M");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "500M");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "  1G");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "  2G");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "  4G");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "  8G");
	waddch(*win, ACS_VLINE);
	wprintw(*win, " 16G");
	waddch(*win, ACS_VLINE);
	wprintw(*win, " 32G");
	waddch(*win, ACS_VLINE);
	wprintw(*win, " 64G");
	waddch(*win, ACS_VLINE);
	uimemdetail(*win, *currow+2, usecolor, memused, memtotal, percent, swap);

	uibanner(*win, cols, "Memory Usage");
	*currow = currowsave;
	uidisplay(*win, currow, cols, lines, winheight);
}

extern void uimemlarge(WINDOW **winin, int winheight, int *currow, int cols, int lines)
{
	return;
}

extern void uimemvirtual(WINDOW **winin, int winheight, int *currow, int cols, int lines)
{
	return;
}

extern void uineterrors(WINDOW **winin, int winheight, int *currow, int cols, int lines)
{
	return;
}

extern void uinetfilesys(WINDOW **winin, int winheight, int *currow, int cols, int lines)
{
	return;
}

static void uinetdetail(WINDOW *win, int currow, int usecolor, unsigned long netin, unsigned long netout, double unitdivisor, char *units, int scale)
{
	char *bytestring = NULL;

	bytestring = uireadablebyteslong(netin);
	mvwprintw(win, currow-1, 2, "In:     %9.9s", bytestring);
	free(bytestring);
	bytestring = NULL;
	bytestring = uireadablebyteslong(netout);
	mvwprintw(win, currow, 2, "Out:    %9.9s", bytestring);
	free(bytestring);
	bytestring = NULL;

	if(usecolor) {
		wattrset(win, COLOR_PAIR(4));
		bytestring = uireadablebyteslong(netin);
		mvwprintw(win, currow-1, 10, "%9.9s", bytestring);
		free(bytestring);
		bytestring = NULL;
		wattrset(win, COLOR_PAIR(1));
        bytestring = uireadablebyteslong(netout);
		mvwprintw(win, currow, 10, "%9.9s", bytestring);
		free(bytestring);
		bytestring = NULL;
		wattrset(win, COLOR_PAIR(0));
	}

	chtype metermark;
	int readquant;
	int writequant;

	if(!scale) {
		if((netin > 0) || (netout > 0)) {
			double nettotal = (double)(netin + netout);
			int tmpquant = (int)floor(log10(nettotal) * 4.9);
			// TODO: this ratio cannot be right for logrithmic output
			readquant = (int)(tmpquant * (netin / (nettotal)));
			writequant = (int)(tmpquant * (netout / (nettotal)));
		} else {
			readquant = 0;
			writequant = 0;
		}
		mvwprintw(win, currow-1, 22, "  LOG");
		mvwprintw(win, currow, 22, "SCALE");
	} else {
		readquant = (int)(floor(netin) / (unitdivisor * 2 * scale));
		writequant = (int)(floor(netout) / (unitdivisor * 2 * scale));
		mvwprintw(win, currow, 24, "%2.2s", units);
	}

	mvwaddch(win, currow, 27, ACS_VLINE);
	wmove(win, currow, 28);

	for(int i = 28; i < 77; ++i){
		if(((i + 3) % 5) == 0) {
			metermark = ACS_VLINE;
		} else {
			metermark = ' ';
		}

		if(readquant) {
			if(usecolor) {
				wattrset(win, COLOR_PAIR(10));
				waddch(win, metermark);
			} else {
				wattron(win, A_STANDOUT);
				waddch(win, ACS_CKBOARD);
				wattroff(win, A_STANDOUT);
			}
			--readquant;
		} else {
			if(writequant) {
				if(usecolor) {
					wattrset(win, COLOR_PAIR(8));
					waddch(win, metermark);
				} else {
					waddch(win, ACS_CKBOARD);
				}
				--writequant;
			} else {
				wattrset(win, COLOR_PAIR(0));
				waddch(win, metermark);
			}
		}
	}
	wattrset(win, COLOR_PAIR(0));
	mvwaddch(win, currow, 77, ACS_VLINE);
}

extern void uinetwork(WINDOW **win, int winheight, int *currow, int cols, int lines, int usecolor, struct sysnet thisnet)
{
	if (*win == NULL) {
		return;
	}
	wclear(*win);

	int currowsave = *currow;
	if(*currow > 0) {
		*currow = 0;
	}

	mvwaddch(*win, *currow+1, 27, ACS_VLINE);
	wprintw(*win, " 10B");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "100B");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "  1K");
	waddch(*win, ACS_VLINE);
	wprintw(*win, " 10K");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "100K");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "  1M");
	waddch(*win, ACS_VLINE);
	wprintw(*win, " 10M");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "100M");
	waddch(*win, ACS_VLINE);
	wprintw(*win, "  1G");
	waddch(*win, ACS_VLINE);
	wprintw(*win, " 10G");
	waddch(*win, ACS_VLINE);
	uinetdetail(*win, *currow+2, usecolor, \
		(thisnet.ibytes - thisnet.oldibytes), \
		(thisnet.obytes - thisnet.oldobytes), \
		0, "", 0);

	uibanner(*win, cols, "Network Usage");
	*currow = currowsave;
	uidisplay(*win, currow, cols, lines, winheight);
}

void uinetlong(WINDOW **win, int winheight, int *currow, int cols, int lines, int itterin, int usecolor, unsigned long *longvals, int valcount)
{
	if (*win == NULL) {
		return;
	}
	// wclear(*win);

	int currowsave = *currow;
	if(*currow > 0) {
		*currow = 0;
	}

	int tempvalue = 0;
	int graphlines = 10 + *currow;
	int offset = 6;

	unsigned long netin = 0;
	unsigned long netout = 0;
	double nettotal = 0;
	int tmpquant = 0;
	int readquant = 0;
	int writequant = 0;

	mvwprintw(*win, *currow+1,  0, " 10G-|");
	mvwprintw(*win, *currow+2,  0, "  1G-|");
	mvwprintw(*win, *currow+3,  0, "100M-|");
	mvwprintw(*win, *currow+4,  0, " 10M-|");
	mvwprintw(*win, *currow+5,  0, "  1M-|");
	mvwprintw(*win, *currow+6,  0, "100K-|");
	mvwprintw(*win, *currow+7,  0, " 10K-|");
	mvwprintw(*win, *currow+8,  0, "  1K-|");
	mvwprintw(*win, *currow+9,  0, "100B-|");
	mvwprintw(*win, *currow+10, 0, " 10B-|");
	mvwvline(*win, 1, offset-1, ACS_VLINE, graphlines);

	for (int j = 0; j < valcount; ++j) {
		tempvalue = j * 2;
		netin = longvals[tempvalue];
		netout = longvals[tempvalue+1];
		readquant = 0;
		writequant = 0;

		nettotal = (double)(netin + netout);
		if(nettotal) {
			tmpquant = (int)floor(log10(nettotal));
			// TODO: this ratio cannot be right for logrithmic output
			readquant = (int)(tmpquant * (netin / (nettotal))) - 0;
			writequant = (int)(tmpquant * (netout / (nettotal))) - 0;
		}

		if(j != itterin) {
			for (int i = graphlines; i > 0; --i) {
				wmove(*win, i, j+offset);

				if(readquant) {
					if(usecolor) {
						wattrset(*win, COLOR_PAIR(10));
						waddch(*win, ACS_VLINE);
					} else {
						wattron(*win, A_STANDOUT);
						waddch(*win, ACS_CKBOARD);
						wattroff(*win, A_STANDOUT);
					}
					--readquant;
				} else {
					if(writequant) {
						if(usecolor) {
							wattrset(*win, COLOR_PAIR(8));
							waddch(*win, ACS_VLINE);
						} else {
							waddch(*win, ACS_CKBOARD);
						}
						--writequant;
					} else {
						wattrset(*win, COLOR_PAIR(0));
						waddch(*win, ' ');
					}
				}
				wattrset(*win, COLOR_PAIR(0));
				wmove(*win, i, itterin+offset+1);
			}
		} else {
			mvwvline(*win, 1, j+offset, ACS_VLINE, graphlines);
		}
	}

	uibanner(*win, cols, "Network Usage");
	*currow = currowsave;
	uidisplay(*win, currow, cols, lines, winheight);
}

void uisys(WINDOW **win, int winheight, int *currow, int cols, int lines, struct syshw hw, struct syskern kern)
{
	if (*win == NULL) {
		return;
	}
	wclear(*win);

	int currowsave = *currow;
	if(*currow > 0) {
		*currow = 0;
	}

	char *bytestringa = uireadablebyteslong(hw.memorysize);
	char *bytestringb = uireadablebyteslong(hw.usermemory);

	mvwprintw(*win, *currow+1, 0, " %s", hw.model);
	mvwprintw(*win, *currow+2, 0, " %s %s", hw.cpuvendor, hw.cpubrand);
	mvwprintw(*win, *currow+3, 0, " %s", kern.version);
	mvwprintw(*win, *currow+4, 0, " OS Release: %s / OS Version: %s", kern.osrelease, kern.osversion);
	mvwprintw(*win, *currow+5, 0, " CPUs: %d (%d cores, %d physical, %d logical)", hw.cpucount, kern.corecount, hw.physicalcpucount, hw.logicalcpucount);
	mvwprintw(*win, *currow+6, 0, " Memory: %9.9s, %9.9s non-kernel in use", bytestringa, bytestringb);
	free(bytestringa);
	free(bytestringb);

	mvwprintw(*win, *currow+8, 0, " Domain   : %s", kern.domainname);
	mvwprintw(*win, *currow+9, 0, " Booted   : %s", kern.boottimestring);

	uibanner(*win, cols, "About This Mac");
	*currow = currowsave;
	uidisplay(*win, currow, cols, lines, winheight);
}

static int comparepercentdes(const void *val1, const void *val2)
{
	struct sysproc **percent1 = (struct sysproc **)val1;
	struct sysproc **percent2 = (struct sysproc **)val2;

	if ((*percent1)->percentage > (*percent2)->percentage) {
		return -1;
	} else if ((*percent1)->percentage < (*percent2)->percentage) {
		return 1;
	} else {
		return 0;
	}
}

static int compareresmemdes(const void *val1, const void *val2)
{
	struct sysproc **percent1 = (struct sysproc **)val1;
	struct sysproc **percent2 = (struct sysproc **)val2;

	if ((*percent1)->residentmem > (*percent2)->residentmem) {
		return -1;
	} else if ((*percent1)->residentmem < (*percent2)->residentmem) {
		return 1;
	} else {
		return 0;
	}
}

void uitop(WINDOW **win, int winheight, int *currow, int cols, int lines, int usecolor, struct sysproc **procs, int processcount, int topmode, bool updateddata, char *user)
{
	if (*win == NULL) {
		return;
	}
	wclear(*win);

	if(!procs){
		return;
	}

	int currowsave = *currow;
	if(*currow > 0) {
		*currow = 0;
	}

	int procstoshow = processcount;
	if(procstoshow > lines) {
		procstoshow = lines - 4;
	}

	wmove(*win, *currow, 1);
	wclrtobot(*win);

	if(updateddata) {
		switch(topmode) {
			case TOP_MODE_A:
			case TOP_MODE_B:
				heapsort(procs, (size_t)processcount, sizeof(struct sysproc *), comparepercentdes);
				break;
			case TOP_MODE_C:
			case TOP_MODE_D:
				heapsort(procs, (size_t)processcount, sizeof(struct sysproc *), compareresmemdes);
				break;
		}
	}

	switch(topmode) {
		case TOP_MODE_A:
		case TOP_MODE_C:
			mvwprintw(*win, *currow+1, 1, "ID     NAME             %%CPU     MEM      PHYS      USER   PGRP   PPID  STATE");
			break;
		case TOP_MODE_B:
		case TOP_MODE_D:
			mvwprintw(*win, *currow+1, 1, "PID    %%CPU   RESSIZE    USER   COMMAND                                      ");
			break;
	}

	char *statustext = NULL;
	char *rmem = NULL;
	char *pmem = NULL;

	char *tmppath = NULL;
	int tmppathlen = 0;
	int appnamebegin = 0;
	int appnameend = 0;
	bool appnamefound = false;

	for (int i = 0; i < procstoshow; i++) {
		switch(procs[i]->status){
			case SIDL:
				statustext = "IDLE";
				break;
			case SRUN:
				if(procs[i]->percentage > 0) {
					statustext = "RUN";
				} else {
					// I made up this classification, I think
					// technically running, but not using any CPU
					statustext = "ZZZ"; // lāze, or as I prefer laz-e
				}
				break;
			case SSLEEP:
				statustext = "SLEEP";
				break;
			case SSTOP:
				statustext = "STOP";
				break;
			case SZOMB:
				statustext = "ZOMB";
				break;
		}

		switch(topmode) {
			case TOP_MODE_A:
			case TOP_MODE_C:
				rmem = uireadablebyteslonglong(procs[i]->residentmem);
				pmem = uireadablebyteslonglong(procs[i]->physicalmem);
				mvwprintw(*win, (*currow + 2 + i), 1, "%-6d %-16.16s%5.1f %9.9s %9.9s %9.9s %-6d %-6d%-5.5s",
					procs[i]->pid,
					procs[i]->name,
					procs[i]->percentage,
					rmem,
					pmem,
					procs[i]->realusername,
					procs[i]->pgid,
					procs[i]->parentpid,
					statustext
					);
				free(rmem);
				free(pmem);

				if(!strcmp(user, procs[i]->realusername)) {
					wattron(*win, A_BOLD);
					mvwprintw(*win, (*currow + 2 + i), 50, "%9.9s", procs[i]->realusername);
					wattroff(*win, A_BOLD);
				}
				break;
			case TOP_MODE_B:
			case TOP_MODE_D:
				rmem = uireadablebyteslonglong(procs[i]->residentmem);
				mvwprintw(*win, (*currow + 2 + i), 1, "%-6d%5.1f %9.9s %9.9s %-45.45s",
					procs[i]->pid,
					procs[i]->percentage,
					rmem,
					procs[i]->realusername,
					procs[i]->path
					);
				if(!strcmp(user, procs[i]->realusername)) {
					wattron(*win, A_BOLD);
					mvwprintw(*win, (*currow + 2 + i), 23, "%9.9s", procs[i]->realusername);
					wattroff(*win, A_BOLD);
				}
				free(rmem);

				if(usecolor) {
					tmppath = procs[i]->path;
					tmppathlen = (int)strlen(tmppath);
					appnamebegin = 0;
					appnameend = tmppathlen;
					appnamefound = false;
					for(int j = tmppathlen; j > 0; --j) {
						if(tmppath[j] == ' ') {
							appnameend = j;
							appnamebegin = 0;
							appnamefound = false;
						} else if(tmppath[j] == '/') {
							if(!appnamefound) {
								appnamebegin = j + 1;
								appnamefound = true;
							}
						}
					}

					wattrset(*win, COLOR_PAIR(5));
					for(int j = appnamebegin; j < appnameend; ++j) {
						mvwprintw(*win, (*currow + 2 + i), (33 + j), "%c", tmppath[j]);
					}
					wattrset(*win, COLOR_PAIR(6));
					for(int j = appnameend; j < tmppathlen; ++j) {
						mvwprintw(*win, (*currow + 2 + i), (33 + j), "%c", tmppath[j]);
					}
					wattrset(*win, COLOR_PAIR(0));
				}
				break;
		}
	}

	uibanner(*win, cols, "Top Processes");
	*currow = currowsave;
	uidisplay(*win, currow, cols, lines, winheight);
}

void uiwarn(WINDOW **win, int winheight, int *currow, int cols, int lines)
{
	if (*win == NULL) {
		return;
	}
	wclear(*win);

	int currowsave = *currow;
	if(*currow > 0) {
		*currow = 0;
	}

	mvwprintw(*win, 1, 0, " Code    Resource            Stats   Now\tWarn\tDanger ");

	uibanner(*win, cols, "Verbose Mode");
	*currow = currowsave;
	uidisplay(*win, currow, cols, lines, winheight);
}
