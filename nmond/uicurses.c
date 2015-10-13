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
	if((*xin + rows + 2) > lines) {
		pnoutrefresh(win, 0, 0, *xin, 1, (lines - 2), (cols - 2));
	} else {
		pnoutrefresh(win, 0, 0, *xin, 1, (*xin + rows), (cols - 2));
	}

	*xin = *xin + rows;
	if((*xin + 1) > lines) {
		mvwprintw(stdscr, lines-1, 10, MSG_WRN_NOT_SHOWN);
	}
}

void uiheader(WINDOW **win, int xin, int usecolor, int blinkon, char *hostname, char *message, double elapsed, time_t timer)
{
	struct tm *tim = localtime(&timer);

	box(stdscr, 0, 0);
	mvwprintw(*win, xin, 3, APPNAME);
	mvwprintw(*win, xin, 11, "%s", hostname);
	if(blinkon) {
		if(strlen(message)) {
			attrset(COLOR_PAIR(8));
			// TODO: should limit to 26 characters
			mvwprintw(*win, xin, 35, "%s", message);
			attrset(COLOR_PAIR(0));
		}
	}
	mvwprintw(*win, xin, 64, "%1.0fs", elapsed);
	mvwprintw(*win, xin, 70, "%02d:%02d.%02d", tim->tm_hour, tim->tm_min, tim->tm_sec);
	wnoutrefresh(stdscr);
}

void uiwelcome(WINDOW **win, int *xin, int cols, int rows, int usecolor, struct syshw hw)
{
	if (*win == NULL) {
		return;
	}
	wclear(*win);

	if(usecolor) {
		wattrset(*win, COLOR_PAIR(2));
	}
	mvwprintw(*win, *xin+0, 18, "                                         __");
	mvwprintw(*win, *xin+1, 18, "    ____   ____ ___   ____   ____   ____/ /");
	mvwprintw(*win, *xin+2, 18, "   / __ \\ / __ `__ \\ / __ \\ / __ \\ / __  / ");
	mvwprintw(*win, *xin+3, 18, "  / / / // / / / / // /_/ // / / // /_/ /  ");
	mvwprintw(*win, *xin+4, 18, " /_/ /_//_/ /_/ /_/ \\____//_/ /_/ \\____/   ");
	mvwprintw(*win, *xin+5, 18, "=======================================    ");
	// mvwprintw(*win, *xin+5, 18, "================================/nmond/    ");
	// mvwprintw(*win, *xin+5, 18, "-------------------------------[nmond]-    ");

	if(usecolor) {
		wattrset(*win, COLOR_PAIR(0));
	}
	// mvwprintw(*win, *xin+1, 45, "For help type H or ...");
	// mvwprintw(*win, *xin+2, 45, " nmond -?  - hint");
	// mvwprintw(*win, *xin+3, 45, " nmond -h  - full");

	if(usecolor) {
		wattrset(*win, COLOR_PAIR(1));
	}
	mvwprintw(*win, *xin+8,  2, "%s", hw.model);
	mvwprintw(*win, *xin+9,  2, "%s", hw.cpubrand);
	mvwprintw(*win, *xin+10, 2, "CPU Count    = %2d   Physcal Cores = %d", hw.cpucount, hw.physicalcpucount);
	mvwprintw(*win, *xin+11, 2, "Hyperthreads = %2d   Virtual CPUs  = %d", hw.hyperthreads, hw.logicalcpucount);
	if(usecolor) {
		wattrset(*win, COLOR_PAIR(0));
	}
	mvwprintw(*win, *xin+13, 2, "Use these keys to toggle statistics on/off:");
	mvwprintw(*win, *xin+14, 2, "  c = CPU               n = Network Usage       - = Reduce refresh delay  ");
	mvwprintw(*win, *xin+15, 2, "  C = CPU, Long-term    r = Top-procs,mem-sort  + = Increase refresh delay");
	mvwprintw(*win, *xin+16, 2, "  d = Disk Usage        R = Top-procs/cmnd,mem  ? = Help                  ");
	mvwprintw(*win, *xin+17, 2, "  i = About this Mac    t = Top-processes                                 ");
	mvwprintw(*win, *xin+18, 2, "  m = Memory Usage      T = Top-procs/command   q = Quit                  ");
	mvwprintw(*win, *xin+20, 1, "To start the same way every time set an NMOND variable: 'export NMOND=cdmnT'");
	pnoutrefresh(*win, 0, 0, *xin, 1, rows-2, cols-2);
	wnoutrefresh(stdscr);
	
	*xin = *xin + 22;
}

void uihelp(WINDOW **win, int *xin, int cols, int rows)
{
	if (*win == NULL) {
		return;
	}

	uibanner(*win, cols, "HELP");
	mvwprintw(*win,  1, 2, "[ a =                               ][ N =  Network Usage, long-term     ]");
	mvwprintw(*win,  2, 2, "[ b = Black & White mode            ][ o =                               ]");
	mvwprintw(*win,  3, 2, "[ c = CPU Load                      ][ r = Top Processes, order by mem   ]");
	mvwprintw(*win,  4, 2, "[ C = CPU Load, long-term           ][ R = Top Processes, command by mem ]");
	mvwprintw(*win,  5, 2, "[ d =                               ][ t = Top Processes, order by proc  ]");
	mvwprintw(*win,  6, 2, "[ D =                               ][ T = Top Processes, command by prc ]");
	mvwprintw(*win,  7, 2, "[ f =                               ][ v =                               ]");
	mvwprintw(*win,  8, 2, "[ F =                               ][ w =                               ]");
	mvwprintw(*win,  9, 2, "[ h = Help                          ][                                   ]");
	mvwprintw(*win, 10, 2, "[ H = Help                          ][                                   ]");
	mvwprintw(*win, 11, 2, "[ i = About This Mac                ][ - = Reduce refresh delay (half)   ]");
	mvwprintw(*win, 12, 2, "[ I =                               ][ + = Increase refresh delay (2x)   ]");
	mvwprintw(*win, 13, 2, "[ k =                               ][                                   ]");
	mvwprintw(*win, 14, 2, "[ m = Memory Usage                  ][ ? = Help                          ]");
	mvwprintw(*win, 15, 2, "[ M =                               ][                                   ]");
	mvwprintw(*win, 16, 2, "[ n = Network Usage                 ][ q = Quit/Exit                     ]");
	mvwprintw(*win, 18, 2, "        %s version %s build %s", APPNAME, VERSION, VERDATE);
	mvwprintw(*win, 19, 2, "              Christopher Stoll, 2015 (%s)", APPURL);
	
	pnoutrefresh(*win, 0, 0, *xin, 1, rows-2, cols-2);
	uidisplay(*win, xin, cols, 20, rows);
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
	mvwprintw(win, row, 27, "|");

	wmove(win, row, 28);
	
	char *metermark = NULL;//malloc(1);
	
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

void uicpu(WINDOW **win, int *xin, int cols, int rows, int usecolor, struct sysres thisres, int show_raw)
{
	if (*win == NULL) {
		return;
	}

	uibanner(*win, cols, "CPU Load");
	mvwprintw(*win, 1, 0, "CPU");
	if(usecolor) {
		wattrset(*win, COLOR_PAIR(4));
		mvwprintw(*win, 1, 4, "User%%");
		wattrset(*win, COLOR_PAIR(1));
		mvwprintw(*win, 1, 10, "Sys %%");
		wattrset(*win, COLOR_PAIR(2));
		mvwprintw(*win, 1, 16, "Nice%%");
		wattrset(*win, COLOR_PAIR(0));
		mvwprintw(*win, 1, 22, "Idle");
	} else {
		mvwprintw(*win, 1, 4, "User%%");
		mvwprintw(*win, 1, 10, "Sys %%");
		mvwprintw(*win, 1, 16, "Wait%%");
		mvwprintw(*win, 1, 22, "Idle");
	}
	mvwprintw(*win, 1, 27, "|0   |  20|    |  40|    |  60|    |  80|    | 100|");

	int cpuno = 0;
	for (cpuno = 0; cpuno < thisres.cpucount; ++cpuno) {
	 	mvwprintw(*win, (2 + cpuno), 77, "|");
		uicpudetail(*win, cpuno, (cpuno + 2), usecolor,
			thisres.cpus[cpuno].percentuser, 
			thisres.cpus[cpuno].percentsys,
			thisres.cpus[cpuno].percentidle,
			thisres.cpus[cpuno].percentnice);
	}

	if (thisres.cpucount > 1) {
		uicpudetail(*win, -1, (cpuno + 2), usecolor,
			thisres.avgpercentuser, 
			thisres.avgpercentsys, 
			thisres.avgpercentidle, 
			thisres.avgpercentnice);
	}
	uidisplay(*win, xin, cols, (cpuno + 3), rows);
}

void uicpulong(WINDOW **win, int *xin, int cols, int rows, int *itterin, int usecolor, struct sysres thisres, bool updategraph)
{
	if (*win == NULL) {
		return;
	}

	mvwprintw(*win, 0, 0, " CPU +---Long-Term-----------------------------------------------------------+");
	if (usecolor){
		wattrset(*win, COLOR_PAIR(4));
		mvwprintw(*win, 0, 27, "User%%");
		wattrset(*win, COLOR_PAIR(1));
		mvwprintw(*win, 0, 35, "System%%");
		wattrset(*win, COLOR_PAIR(2));
		mvwprintw(*win, 0, 45, "Nice%%");
		wattrset(*win, COLOR_PAIR(0));
	}
	mvwprintw(*win, 1, 0, "100%%-|");
	mvwprintw(*win, 2, 1, "95%%-|");
	mvwprintw(*win, 3, 1, "90%%-|");
	mvwprintw(*win, 4, 1, "85%%-|");
	mvwprintw(*win, 5, 1, "80%%-|");
	mvwprintw(*win, 6, 1, "75%%-|");
	mvwprintw(*win, 7, 1, "70%%-|");
	mvwprintw(*win, 8, 1, "65%%-|");
	mvwprintw(*win, 9, 1, "60%%-|");
	mvwprintw(*win, 10, 1, "55%%-|");
	mvwprintw(*win, 11, 1, "50%%-|");
	mvwprintw(*win, 12, 1, "45%%-|");
	mvwprintw(*win, 13, 1, "40%%-|");
	mvwprintw(*win, 14, 1, "35%%-|");
	mvwprintw(*win, 15, 1, "30%%-|");
	mvwprintw(*win, 16, 1, "25%%-|");
	mvwprintw(*win, 17, 1, "20%%-|");
	mvwprintw(*win, 18, 1, "15%%-|");
	mvwprintw(*win, 19, 1, "10%%-|");
	mvwprintw(*win, 20, 1, " 5%%-|");
	
	if(updategraph) {
		int graphcols = 70;
		int graphrows = 20;
		int offset = 6;

		char *metermark = NULL;
		char *blankmark = NULL;
		char *leadermark = NULL;

		int userquant = (int)(round(thisres.avgpercentuser) / 5);
		int systquant = (int)(round(thisres.avgpercentsys) / 5);
		int nicequant = (int)(round(thisres.avgpercentnice) / 5);

		for (int i = graphrows; i > 0; --i) {
			wmove(*win, i, *itterin+offset);
			
			if((i > 1) && (((i - 1) % 4) == 0)) {
				metermark = "+";
				blankmark = "-";
				leadermark = "+";
			} else {
				metermark = "|";
				blankmark = " ";
				leadermark = "|";
			}

			if(userquant) {
				if(usecolor) {
					wattrset(*win, COLOR_PAIR(10));
					wprintw(*win, metermark);
				} else {
					wprintw(*win, "U");
				}
				--userquant;
			} else {
				if(systquant) {
					if(usecolor) {
						wattrset(*win, COLOR_PAIR(8));
						wprintw(*win, metermark);
					} else {
						wprintw(*win, "S");
					}
					--systquant;
				} else {
					if(nicequant) {
						if(usecolor) {
							wattrset(*win, COLOR_PAIR(9));
							wprintw(*win, metermark);
						} else {
							wprintw(*win, "N");
						}
						--nicequant;
					} else {
						wattrset(*win, COLOR_PAIR(0));
						wprintw(*win, blankmark);
					}
				}
			}
			wattrset(*win, COLOR_PAIR(0));
			wmove(*win, i, *itterin+offset+1);
			wprintw(*win, leadermark);
		}

		*itterin += 1;
		if(*itterin > graphcols) {
			*itterin = 0;
		}
	}
	uidisplay(*win, xin, cols, 21, rows);
}

static void uidiskdetail(WINDOW *win, int x, int usecolor, unsigned long diskr, unsigned long diskw, double unitdivisor, char *units, int scale)
{
	char *bytestring = NULL;

	bytestring = uireadablebyteslong(diskr);
	mvwprintw(win, 1, 2, "Reads:  %9.9s", bytestring);
	free(bytestring);
	bytestring = NULL;
	bytestring = uireadablebyteslong(diskw);
	mvwprintw(win, 2, 2, "Writes: %9.9s", bytestring);
	free(bytestring);
	bytestring = NULL;

	if(usecolor) {
		wattrset(win, COLOR_PAIR(4));
		bytestring = uireadablebyteslong(diskr);
		mvwprintw(win, 1, 10, "%9.9s", bytestring);
		free(bytestring);
		bytestring = NULL;
		wattrset(win, COLOR_PAIR(1));
        bytestring = uireadablebyteslong(diskw);
		mvwprintw(win, 2, 10, "%9.9s", bytestring);
		free(bytestring);
		bytestring = NULL;
		wattrset(win, COLOR_PAIR(0));
	}

	char *metermark = "#";
	int readquant;
	int writequant;
	double disktotal;

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
		mvwprintw(win, 1, 22, "  LOG");
		mvwprintw(win, 2, 22, "SCALE");
	} else {
		readquant = (int)(floor(diskr) / (unitdivisor * 2 * scale));
		writequant = (int)(floor(diskw) / (unitdivisor * 2 * scale));
		mvwprintw(win, x, 24, "%2.2s", units);
	}

	mvwprintw(win, x, 27, "|");
	wmove(win, x, 28);
	
	for(int i = 28; i < 77; ++i){
		if(((i + 3) % 5) == 0) {
			metermark = "|";
		} else {
			metermark = " ";
		}

		if(readquant) {
			if(usecolor) {
				wattrset(win, COLOR_PAIR(10));
				wprintw(win, metermark);
			} else {
				wprintw(win, "#");
			}
			--readquant;
		} else {
			if(writequant) {
				if(usecolor) {
					wattrset(win, COLOR_PAIR(8));
					wprintw(win, metermark);
				} else {
					wprintw(win, "#");
				}
				--writequant;
			} else {
				wattrset(win, COLOR_PAIR(0));
				wprintw(win, metermark);
			}
		}
	}
	wattrset(win, COLOR_PAIR(0));
	mvwprintw(win, x, 77, "|");
}

extern void uidisks(WINDOW **win, int *xin, int cols, int rows, int usecolor, unsigned int diskr, unsigned int diskw)
{
	if (*win == NULL) {
		return;
	}

	unsigned int disktotal = diskr + diskw;

	uibanner(*win, cols, "Disk Use");
	if(DISK_METER_MODE == DISK_METER_LOG) {
		mvwprintw(*win, 1, 27, "| 10B|100B|  1K| 10K|100K|  1M| 10M|100M|  1G| 10G|");
 		mvwprintw(*win, 2, 77, "|");
 		uidiskdetail(*win, 2, usecolor, diskr, diskw, 0, "", 0);

	} else if(DISK_METER_MODE == DISK_METER_SCALE) {
	 	if(disktotal <= (BYTES_IN_KB * 100)) {
	 		mvwprintw(*win, 1, 27, "|0   |  20|    |  40|    |  60|    |  80|    | 100|");
	 		mvwprintw(*win, 2, 77, "|");
	 		uidiskdetail(*win, 2, usecolor, diskr, diskw, BYTES_IN_KB, "KB", 1);
	 	
	 	} else if(disktotal <= (BYTES_IN_KB * 1000)) {
	 		mvwprintw(*win, 1, 27, "|0   | 200|    | 400|    | 600|    | 800|    |1000|");
	 		mvwprintw(*win, 2, 77, "|");
	 		uidiskdetail(*win, 2, usecolor, diskr, diskw, BYTES_IN_KB, "KB", 10);
	 	
	 	} else {
	 		if(disktotal <= (BYTES_IN_MB * 100)) {
	 			mvwprintw(*win, 1, 27, "|0   |  20|    |  40|    |  60|    |  80|    | 100|");
		 		mvwprintw(*win, 2, 77, "|");
		 		uidiskdetail(*win, 2, usecolor, diskr, diskw, BYTES_IN_MB, "MB", 1);
	 		
	 		} else if(disktotal <= (BYTES_IN_MB * 1000)) {
		 		mvwprintw(*win, 1, 27, "|0   | 200|    | 400|    | 600|    | 800|    |1000|");
		 		mvwprintw(*win, 2, 77, "|");
		 		uidiskdetail(*win, 2, usecolor, diskr, diskw, BYTES_IN_MB, "MB", 10);
		 	
		 	} else {
		 		if(disktotal <= (BYTES_IN_GB * 100)) {
		 			mvwprintw(*win, 1, 27, "|0   |  20|    |  40|    |  60|    |  80|    | 100|");
			 		mvwprintw(*win, 2, 77, "|");
			 		uidiskdetail(*win, 2, usecolor, diskr, diskw, BYTES_IN_GB, "GB", 1);

			 	} else {
			 		mvwprintw(*win, 1, 27, "|0   | 200|    | 400|    | 600|    | 800|    |1000|");
			 		mvwprintw(*win, 2, 77, "|");
			 		uidiskdetail(*win, 2, usecolor, diskr, diskw, BYTES_IN_GB, "GB", 10);
			 	}
			}
	 	}
	} else {
		mvwprintw(*win, 1, 27, "|0   |  20|    |  40|    |  60|    |  80|    | 100|");
 		mvwprintw(*win, 2, 77, "|");
 		uidiskdetail(*win, 2, usecolor, diskr, diskw, BYTES_IN_MB, "MB", 1);
	}
	uidisplay(*win, xin, cols, 3, rows);
}

void uidisklong(WINDOW **win, int *xin, int cols, int rows, int *itterin, int usecolor, unsigned int diskr, unsigned int diskw, bool updategraph)
{
	if (*win == NULL) {
		return;
	}

	mvwprintw(*win, 0, 0, "DISK +---Long-Term-----------------------------------------------------------+");
	if (usecolor){
		wattrset(*win, COLOR_PAIR(4));
		mvwprintw(*win, 0, 27, "In");
		wattrset(*win, COLOR_PAIR(1));
		mvwprintw(*win, 0, 35, "Out");
		// wattrset(*win, COLOR_PAIR(2));
		// mvwprintw(*win, 0, 45, "Nice%%");
		wattrset(*win, COLOR_PAIR(0));
	}
	mvwprintw(*win,  1, 0, " 10G-|");
	mvwprintw(*win,  2, 0, "     |");
	mvwprintw(*win,  3, 0, "  1G-|");
	mvwprintw(*win,  4, 0, "     |");
	mvwprintw(*win,  5, 0, "100M-|");
	mvwprintw(*win,  6, 0, "     |");
	mvwprintw(*win,  7, 0, " 10M-|");
	mvwprintw(*win,  8, 0, "     |");
	mvwprintw(*win,  9, 0, "  1M-|");
	mvwprintw(*win, 10, 0, "     |");
	mvwprintw(*win, 11, 0, "100K-|");
	mvwprintw(*win, 12, 0, "     |");
	mvwprintw(*win, 13, 0, " 10K-|");
	mvwprintw(*win, 14, 0, "     |");
	mvwprintw(*win, 15, 0, "  1K-|");
	mvwprintw(*win, 16, 0, "     |");
	mvwprintw(*win, 17, 0, "100B-|");
	mvwprintw(*win, 18, 0, "     |");
	mvwprintw(*win, 19, 0, " 10B-|");
	mvwprintw(*win, 20, 0, "     |");
	
	if(updategraph) {
		int graphcols = 70;
		int graphrows = 20;
		int offset = 6;

		char *metermark = NULL;
		char *blankmark = NULL;
		char *leadermark = NULL;

		int readquant = 0;
		int writequant = 0;
		int zzzzquant = 0;

		double disktotal = (double)(diskr + diskw);

		int tmpquant = 0;
		if(disktotal) {
			tmpquant = (int)floor(log10(disktotal) * 2);
			// TODO: this ratio cannot be right for logrithmic output
			readquant = (int)(tmpquant * (diskr / (disktotal))) - 0;
			writequant = (int)(tmpquant * (diskw / (disktotal))) - 0;
		}

		for (int i = graphrows; i > 0; --i) {
			wmove(*win, i, *itterin+offset);
			
			if((i > 1) && (((i - 1) % 4) == 0)) {
				metermark = "+";
				blankmark = "-";
				leadermark = "+";
			} else {
				metermark = "|";
				blankmark = " ";
				leadermark = "|";
			}

			if(readquant) {
				if(usecolor) {
					wattrset(*win, COLOR_PAIR(10));
					wprintw(*win, metermark);
				} else {
					wprintw(*win, "R");
				}
				--readquant;
			} else {
				if(writequant) {
					if(usecolor) {
						wattrset(*win, COLOR_PAIR(8));
						wprintw(*win, metermark);
					} else {
						wprintw(*win, "W");
					}
					--writequant;
				} else {
					if(zzzzquant) {
						if(usecolor) {
							wattrset(*win, COLOR_PAIR(9));
							wprintw(*win, metermark);
						} else {
							wprintw(*win, "#");
						}
						--zzzzquant;
					} else {
						wattrset(*win, COLOR_PAIR(0));
						wprintw(*win, blankmark);
					}
				}
			}
			wattrset(*win, COLOR_PAIR(0));
			wmove(*win, i, *itterin+offset+1);
			wprintw(*win, leadermark);
		}

		*itterin += 1;
		if(*itterin > graphcols) {
			*itterin = 0;
		}
	}
	uidisplay(*win, xin, cols, 21, rows);
}

extern void uidiskgroup(WINDOW **winin, int *xin, int cols, int rows)
{
	return;
}

extern void uidiskmap(WINDOW **winin, int *xin, int cols, int rows)
{
	return;
}

extern void uifilesys(WINDOW **winin, int *xin, int cols, int rows)
{
	return;
}

extern void uikernel(WINDOW **winin, int *xin, int cols, int rows)
{
	return;
}

static void uimemdetail(WINDOW *win, int usecolor, unsigned long long used, unsigned long long total, double percent)
{
	char *bytestring = NULL;

	bytestring = uireadablebyteslonglong(total);
	mvwprintw(win, 1, 2, "Total: %9.9s", bytestring);
	free(bytestring);
	bytestring = NULL;
	bytestring = uireadablebyteslonglong(used);
	mvwprintw(win, 2, 2, "Used:  %9.9s  %5.2f%%", bytestring, percent);
	free(bytestring);
	bytestring = NULL;

	if(usecolor) {
		wattrset(win, COLOR_PAIR(4));
		mvwprintw(win, 2, 20, "%5.2f%%", percent);
		wattrset(win, COLOR_PAIR(0));
	}
	mvwprintw(win, 2, 27, "|");
	wmove(win, 2, 28);

	char *metermark = "#";
	int usedquant = (int)(floor(percent) / 2) - 1;
	
	for(int i=28; i<77; ++i){
		if(((i + 3) % 5) == 0) {
			metermark = "|";
		} else {
			metermark = " ";
		}

		if(usedquant >= 0) {
			if(usecolor) {
				wattrset(win, COLOR_PAIR(10));
				wprintw(win, metermark);
			} else {
				wprintw(win, "#");
			}
			--usedquant;
		} else {
			wattrset(win, COLOR_PAIR(0));
			wprintw(win, metermark);
		}
	}
	wattrset(win, COLOR_PAIR(0));
	mvwprintw(win, 2, 77, "|");
}

extern void uimemory(WINDOW **win, int *xin, int cols, int rows, int usecolor, unsigned long long memused, unsigned long long memtotal)
{
	if (*win == NULL) {
		return;
	}

	double percent = (double)((memused / 100.0) / (memtotal / 100.0) * 100);
	uibanner(*win, cols, "Memory Use");
	mvwprintw(*win, 1, 27, "|0   |  20|    |  40|    |  60|    |  80|    | 100|");
 	mvwprintw(*win, 2, 77, "|");
	uimemdetail(*win, usecolor, memused, memtotal, percent);
	uidisplay(*win, xin, cols, 3, rows);
}

extern void uimemlarge(WINDOW **winin, int *xin, int cols, int rows)
{
	return;
}

extern void uimemvirtual(WINDOW **winin, int *xin, int cols, int rows)
{
	return;
}

extern void uineterrors(WINDOW **winin, int *xin, int cols, int rows)
{
	return;
}

extern void uinetfilesys(WINDOW **winin, int *xin, int cols, int rows)
{
	return;
}

static void uinetdetail(WINDOW *win, int x, int usecolor, unsigned long netin, unsigned long netout, double unitdivisor, char *units, int scale)
{
	char *bytestring = NULL;

	bytestring = uireadablebyteslong(netin);
	mvwprintw(win, 1, 2, "In:     %9.9s", bytestring);
	free(bytestring);
	bytestring = NULL;
	bytestring = uireadablebyteslong(netout);
	mvwprintw(win, 2, 2, "Out:    %9.9s", bytestring);
	free(bytestring);
	bytestring = NULL;

	if(usecolor) {
		wattrset(win, COLOR_PAIR(4));
		bytestring = uireadablebyteslong(netin);
		mvwprintw(win, 1, 10, "%9.9s", bytestring);
		free(bytestring);
		bytestring = NULL;
		wattrset(win, COLOR_PAIR(1));
        bytestring = uireadablebyteslong(netout);
		mvwprintw(win, 2, 10, "%9.9s", bytestring);
		free(bytestring);
		bytestring = NULL;
		wattrset(win, COLOR_PAIR(0));
	}

	char *metermark = "#";
	int readquant;
	int writequant;
	double nettotal;

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
		mvwprintw(win, 1, 22, "  LOG");
		mvwprintw(win, 2, 22, "SCALE");
	} else {
		readquant = (int)(floor(netin) / (unitdivisor * 2 * scale));
		writequant = (int)(floor(netout) / (unitdivisor * 2 * scale));
		mvwprintw(win, x, 24, "%2.2s", units);
	}

	mvwprintw(win, x, 27, "|");
	wmove(win, x, 28);
	
	for(int i = 28; i < 77; ++i){
		if(((i + 3) % 5) == 0) {
			metermark = "|";
		} else {
			metermark = " ";
		}

		if(readquant) {
			if(usecolor) {
				wattrset(win, COLOR_PAIR(10));
				wprintw(win, metermark);
			} else {
				wprintw(win, "#");
			}
			--readquant;
		} else {
			if(writequant) {
				if(usecolor) {
					wattrset(win, COLOR_PAIR(8));
					wprintw(win, metermark);
				} else {
					wprintw(win, "#");
				}
				--writequant;
			} else {
				wattrset(win, COLOR_PAIR(0));
				wprintw(win, metermark);
			}
		}
	}
	wattrset(win, COLOR_PAIR(0));
	mvwprintw(win, x, 77, "|");
}

extern void uinetwork(WINDOW **win, int *xin, int cols, int rows, int usecolor, struct sysnet thisnet)
{
	if (*win == NULL) {
		return;
	}

	uibanner(*win, cols, "Network Use");
	if(DISK_METER_MODE == DISK_METER_LOG) {
		mvwprintw(*win, 1, 27, "| 10B|100B|  1K| 10K|100K|  1M| 10M|100M|  1G| 10G|");
 		mvwprintw(*win, 2, 77, "|");
 		uinetdetail(*win, 2, usecolor, \
 			(thisnet.ibytes - thisnet.oldibytes), \
 			(thisnet.obytes - thisnet.oldobytes), \
 			0, "", 0);
	}
	uidisplay(*win, xin, cols, 3, rows);
}

void uinetlong(WINDOW **win, int *xin, int cols, int rows, int *itterin, int usecolor, struct sysnet thisnet, bool updategraph)
{
	if (*win == NULL) {
		return;
	}

	mvwprintw(*win, 0, 0, " NET +---Long-Term-----------------------------------------------------------+");
	if (usecolor){
		wattrset(*win, COLOR_PAIR(4));
		mvwprintw(*win, 0, 27, "In");
		wattrset(*win, COLOR_PAIR(1));
		mvwprintw(*win, 0, 35, "Out");
		// wattrset(*win, COLOR_PAIR(2));
		// mvwprintw(*win, 0, 45, "Nice%%");
		wattrset(*win, COLOR_PAIR(0));
	}
	mvwprintw(*win,  1, 0, " 10G-|");
	mvwprintw(*win,  2, 0, "     |");
	mvwprintw(*win,  3, 0, "  1G-|");
	mvwprintw(*win,  4, 0, "     |");
	mvwprintw(*win,  5, 0, "100M-|");
	mvwprintw(*win,  6, 0, "     |");
	mvwprintw(*win,  7, 0, " 10M-|");
	mvwprintw(*win,  8, 0, "     |");
	mvwprintw(*win,  9, 0, "  1M-|");
	mvwprintw(*win, 10, 0, "     |");
	mvwprintw(*win, 11, 0, "100K-|");
	mvwprintw(*win, 12, 0, "     |");
	mvwprintw(*win, 13, 0, " 10K-|");
	mvwprintw(*win, 14, 0, "     |");
	mvwprintw(*win, 15, 0, "  1K-|");
	mvwprintw(*win, 16, 0, "     |");
	mvwprintw(*win, 17, 0, "100B-|");
	mvwprintw(*win, 18, 0, "     |");
	mvwprintw(*win, 19, 0, " 10B-|");
	mvwprintw(*win, 20, 0, "     |");
	
	if(updategraph) {
		int graphcols = 70;
		int graphrows = 20;
		int offset = 6;

		char *metermark = NULL;
		char *blankmark = NULL;
		char *leadermark = NULL;

		int readquant = 0;
		int writequant = 0;
		int zzzzquant = 0;

		unsigned long netin = (thisnet.ibytes - thisnet.oldibytes);
		unsigned long netout = (thisnet.obytes - thisnet.oldobytes);
		double nettotal = (double)(netin + netout);

		int tmpquant = 0;
		if(nettotal) {
			tmpquant = (int)floor(log10(nettotal) * 2);
			// TODO: this ratio cannot be right for logrithmic output
			readquant = (int)(tmpquant * (netin / (nettotal))) - 0;
			writequant = (int)(tmpquant * (netout / (nettotal))) - 0;
		}

		for (int i = graphrows; i > 0; --i) {
			wmove(*win, i, *itterin+offset);
			
			if((i > 1) && (((i - 1) % 4) == 0)) {
				metermark = "+";
				blankmark = "-";
				leadermark = "+";
			} else {
				metermark = "|";
				blankmark = " ";
				leadermark = "|";
			}

			if(readquant) {
				if(usecolor) {
					wattrset(*win, COLOR_PAIR(10));
					wprintw(*win, metermark);
				} else {
					wprintw(*win, "R");
				}
				--readquant;
			} else {
				if(writequant) {
					if(usecolor) {
						wattrset(*win, COLOR_PAIR(8));
						wprintw(*win, metermark);
					} else {
						wprintw(*win, "W");
					}
					--writequant;
				} else {
					if(zzzzquant) {
						if(usecolor) {
							wattrset(*win, COLOR_PAIR(9));
							wprintw(*win, metermark);
						} else {
							wprintw(*win, "#");
						}
						--zzzzquant;
					} else {
						wattrset(*win, COLOR_PAIR(0));
						wprintw(*win, blankmark);
					}
				}
			}
			wattrset(*win, COLOR_PAIR(0));
			wmove(*win, i, *itterin+offset+1);
			wprintw(*win, leadermark);
		}

		*itterin += 1;
		if(*itterin > graphcols) {
			*itterin = 0;
		}
	}
	uidisplay(*win, xin, cols, 21, rows);
}

void uisys(WINDOW **win, int *xin, int cols, int rows, struct syshw hw, struct syskern kern)
{
	if (*win == NULL) {
		return;
	}

	char *bytestringa = uireadablebyteslong(hw.memorysize);
	char *bytestringb = uireadablebyteslong(hw.usermemory);

	uibanner(*win, cols, "About This Mac");
	mvwprintw(*win, 1, 2, "%s", hw.model);
	mvwprintw(*win, 2, 2, "%s %s", hw.cpuvendor, hw.cpubrand);
	mvwprintw(*win, 3, 2, "%s", kern.version);
	mvwprintw(*win, 4, 2, "OS Release: %s / OS Version: %s", kern.osrelease, kern.osversion);
	mvwprintw(*win, 5, 2, "CPUs: %d (%d cores, %d physical, %d logical)", hw.cpucount, kern.corecount, hw.physicalcpucount, hw.logicalcpucount);
	mvwprintw(*win, 6, 2, "Memory: %9.9s, %9.9s non-kernel in use", bytestringa, bytestringb);
	free(bytestringa);
	free(bytestringb);
	
	mvwprintw(*win, 8, 2, "Domain   : %s", kern.domainname);
	mvwprintw(*win, 9, 2, "Booted   : %s", kern.boottimestring);
	uidisplay(*win, xin, cols, 10, rows);
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

void uitop(WINDOW **win, int *xin, int cols, int rows, int usecolor, struct sysproc **procs, int processcount, int topmode, bool updateddata, char *user)
{
	if (*win == NULL) {
		return;
	}

	if(!procs){
		return;
	}

	int procstoshow = processcount;
	if(procstoshow > rows) {
		procstoshow = rows - 4;
	}

	wmove(*win, 1, 1);
	wclrtobot(*win);

	// TODO: this is not working???
	uibanner(*win, cols, "Top Processes");
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
			mvwprintw(*win, 1, 1, "ID     NAME             %%CPU     MEM      PHYS      USER   PGRP   PPID  STATE");
			break;
		case TOP_MODE_B:
		case TOP_MODE_D:
			mvwprintw(*win, 1, 1, "PID    %%CPU   RESSIZE    USER   COMMAND                                      ");
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
				mvwprintw(*win, (i + 2), 1, "%-6d %-16.16s%5.1f %9.9s %9.9s %9.9s %-6d %-6d%-5.5s", 
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
					mvwprintw(*win, (i + 2), 50, "%9.9s", procs[i]->realusername);
					wattroff(*win, A_BOLD);
				}
				break;
			case TOP_MODE_B:
			case TOP_MODE_D:
				rmem = uireadablebyteslonglong(procs[i]->residentmem);
				mvwprintw(*win, (i + 2), 1, "%-6d%5.1f %9.9s %9.9s %-45.45s", 
					procs[i]->pid,
					procs[i]->percentage,
					rmem,
					procs[i]->realusername,
					procs[i]->path
					);
				if(!strcmp(user, procs[i]->realusername)) {
					wattron(*win, A_BOLD);
					mvwprintw(*win, (i + 2), 23, "%9.9s", procs[i]->realusername);
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
						mvwprintw(*win, (i + 2), (33 + j), "%c", tmppath[j]);
					}
					wattrset(*win, COLOR_PAIR(6));
					for(int j = appnameend; j < tmppathlen; ++j) {
						mvwprintw(*win, (i + 2), (33 + j), "%c", tmppath[j]);
					} 
					wattrset(*win, COLOR_PAIR(0));
				}
				break;
		}
	}
	uidisplay(*win, xin, cols, 27, rows);
}

void uiwarn(WINDOW **win, int *xin, int cols, int rows)
{
	if (*win == NULL) {
		return;
	}

	uibanner(*win, cols, "Verbose Mode");
	mvwprintw(*win, 1, 0, " Code    Resource            Stats   Now\tWarn\tDanger ");
	uidisplay(*win, xin, cols, 4, rows);
}
