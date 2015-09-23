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

static inline double bytestogb(int inbytes)
{
	return (double)inbytes / BYTES_IN_GB;
}
static inline double bytestogb64(int64_t inbytes)
{
	return (double)inbytes / BYTES_IN_GB;
}

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

void uiheader(int *xin, int usecolor, int blinkon, char *hostname, char *message, double elapsed, time_t timer)
{
	struct tm *tim = localtime(&timer);

	box(stdscr, 0, 0);
	mvprintw(*xin, 2, APPNAME);
	mvprintw(*xin, 10, "%s", hostname);
	if(blinkon) {
		if(strlen(message)) {
			attrset(COLOR_PAIR(8));
			// TODO: should limit to 26 characters
			mvprintw(*xin, 35, "%s", message);
			attrset(COLOR_PAIR(0));
		}
	}
	mvprintw(*xin, 64, "%1.0fs", elapsed);
	mvprintw(*xin, 70, "%02d:%02d.%02d", tim->tm_hour, tim->tm_min, tim->tm_sec);
	wnoutrefresh(stdscr);

	*xin = *xin + 1;
}

void uiwelcome(WINDOW **winin, int *xin, int cols, int rows, int usecolor, struct syshw hw)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}

	if(usecolor) {
		wattrset(win, COLOR_PAIR(2));
	}
	mvwprintw(win, *xin+1, 1, "                                         __");
	mvwprintw(win, *xin+2, 1, "    ____   ____ ___   ____   ____   ____/ /");
	mvwprintw(win, *xin+3, 1, "   / __ \\ / __ `__ \\ / __ \\ / __ \\ / __  / ");
	mvwprintw(win, *xin+4, 1, "  / / / // / / / / // /_/ // / / // /_/ /  ");
	mvwprintw(win, *xin+5, 1, " /_/ /_//_/ /_/ /_/ \\____//_/ /_/ \\____/   ");
	mvwprintw(win, *xin+6, 1, "=======================================    ");

	if(usecolor) {
		wattrset(win, COLOR_PAIR(0));
	}
	mvwprintw(win, *xin+1, 45, "For help type H or ...");
	mvwprintw(win, *xin+2, 45, " nmond -?  - hint");
	mvwprintw(win, *xin+3, 45, " nmond -h  - full");
	mvwprintw(win, *xin+5, 45, "To start the same way every time");
	mvwprintw(win, *xin+6, 45, " set the NMOND shell variable");
	if(usecolor) {
		wattrset(win, COLOR_PAIR(1));
	}
	mvwprintw(win, *xin+8,  2, "%s", hw.model);
	mvwprintw(win, *xin+9,  2, "%s", hw.cpubrand);
	mvwprintw(win, *xin+10, 2, "CPU Count    = %2d   Physcal Cores = %d", hw.cpucount, hw.physicalcpucount);
	mvwprintw(win, *xin+11, 2, "Hyperthreads = %2d   Virtual CPUs  = %d", hw.hyperthreads, hw.logicalcpucount);
	if(usecolor) {
		wattrset(win, COLOR_PAIR(0));
	}
	mvwprintw(win, *xin+15, 3, "Use these keys to toggle statistics on/off:");
	mvwprintw(win, *xin+16, 3, "  c = CPU               t = Top-processes     - = Reduce refresh delay  ");
	mvwprintw(win, *xin+17, 3, "  C = CPU, Long-term    T = Top-procs,command + = Increase refresh delay");
	mvwprintw(win, *xin+18, 3, "    =                     =                   ? = Help                  ");
	mvwprintw(win, *xin+19, 3, "    =                     =                                             ");
	mvwprintw(win, *xin+20, 3, "  i = About this Mac      =                   q = Quit                  ");
	pnoutrefresh(win, 0, 0, *xin, 1, rows-2, cols-2);
	wnoutrefresh(stdscr);
	
	*xin = *xin + 22;
}

void uihelp(WINDOW **winin, int *xin, int cols, int rows)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}

	uibanner(win, cols, "HELP");
	mvwprintw(win,  1, 2, "[ a =                               ][ N =                               ]");
	mvwprintw(win,  2, 2, "[ b = Black & White mode            ][ o =                               ]");
	mvwprintw(win,  3, 2, "[ c = CPU Load                      ][ t = Top Processes                 ]");
	mvwprintw(win,  4, 2, "[ C = CPU Load, long-term           ][ T = Top Processes, show command   ]");
	mvwprintw(win,  5, 2, "[ d =                               ][ v =                               ]");
	mvwprintw(win,  6, 2, "[ D =                               ][ w =                               ]");
	mvwprintw(win,  7, 2, "[ f =                               ][                                   ]");
	mvwprintw(win,  8, 2, "[ F =                               ][                                   ]");
	mvwprintw(win,  9, 2, "[ h = Help                          ][                                   ]");
	mvwprintw(win, 10, 2, "[ H = Help                          ][                                   ]");
	mvwprintw(win, 11, 2, "[ i = About This Mac                ][ - = Reduce refresh delay (half)   ]");
	mvwprintw(win, 12, 2, "[ I =                               ][ + = Increase refresh delay (2x)   ]");
	mvwprintw(win, 13, 2, "[ k =                               ][                                   ]");
	mvwprintw(win, 14, 2, "[ m =                               ][ ? = Help                          ]");
	mvwprintw(win, 15, 2, "[ M =                               ][                                   ]");
	mvwprintw(win, 16, 2, "[ n =                               ][ q = Quit/Exit                     ]");
	mvwprintw(win, 18, 2, "        %s version %s build %s", APPNAME, VERSION, VERDATE);
	mvwprintw(win, 19, 2, "              Christopher Stoll, 2015 (%s)", APPURL);
	
	pnoutrefresh(win, 0, 0, *xin, 1, rows-2, cols-2);
	uidisplay(win, xin, cols, 20, rows);
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
	// free(metermark);
	wattrset(win, COLOR_PAIR(0));
	mvwprintw(win, row, 77, "|");
}

void uicpu(WINDOW **winin, int *xin, int cols, int rows, int usecolor, struct sysres thisres, int show_raw)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}

	uibanner(win, cols, "CPU Load");
	mvwprintw(win, 1, 0, "CPU");
	if(usecolor) {
		wattrset(win, COLOR_PAIR(4));
		mvwprintw(win, 1, 4, "User%%");
		wattrset(win, COLOR_PAIR(1));
		mvwprintw(win, 1, 10, "Sys %%");
		wattrset(win, COLOR_PAIR(2));
		mvwprintw(win, 1, 16, "Nice%%");
		wattrset(win, COLOR_PAIR(0));
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
	uidisplay(win, xin, cols, (cpuno + 3), rows);
}

void uicpulong(WINDOW **winin, int *xin, int cols, int rows, int *itterin, int usecolor, struct sysres thisres, bool updategraph)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}
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
	
	if(updategraph) {
		int graphcols = 70;
		int graphrows = 20;
		int offset = 6;

		char *metermark = (char*)malloc(sizeof(char));
		char *blankmark = (char*)malloc(sizeof(char));
		char *leadermark = (char*)malloc(sizeof(char));

		int userquant = (int)(round(thisres.avgpercentuser) / 5);
		int systquant = (int)(round(thisres.avgpercentsys) / 5);
		int nicequant = (int)(round(thisres.avgpercentnice) / 5);

		for (int i = graphrows; i > 0; --i) {
			wmove(win, i, itteration+offset);
			
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
						wprintw(win, blankmark);
					}
				}
			}
			wattrset(win, COLOR_PAIR(0));
			wmove(win, i, itteration+offset+1);
			wprintw(win, leadermark);
		}
		// free(metermark);
		// free(blankmark);
	// free(leadermark);

		++itteration;
		if(itteration > graphcols) {
			itteration = 0;
		}
	}
	uidisplay(win, xin, cols, 21, rows);

	*itterin = itteration;
}

extern void uidisks(WINDOW **winin, int *xin, int cols, int rows)
{
/*
				if(show_disk) {
					BANNER(paddisk,"Disk I/O");
					switch(disk_mode) {
						case DISK_MODE_PARTITIONS: mvwprintw(paddisk, 0, 12, "/Users/stollcri/Documents/code/c/nmond/dbg/partitions");break;
						case DISK_MODE_DISKSTATS:  mvwprintw(paddisk, 0, 12, "/Users/stollcri/Documents/code/c/nmond/dbg/diskstats");break;
						case DISK_MODE_IO:         mvwprintw(paddisk, 0, 12, "/Users/stollcri/Documents/code/c/nmond/dbg/stat+disk_io");break;
					}
					mvwprintw(paddisk,0, 31, "mostly in KB/s");
					mvwprintw(paddisk,0, 50, "Warning:contains duplicates");
					switch (show_disk) {
						case SHOW_DISK_STATS: 
							mvwprintw(paddisk,1, 0, "DiskName Busy    Read    Write       Xfers   Size  Peak%%  Peak-RW    InFlight ");
							break;
						case SHOW_DISK_GRAPH: 
							mvwprintw(paddisk,1, 0, "DiskName Busy  ");
							COLOUR wattrset(paddisk,COLOR_PAIR(6));
							mvwprintw(paddisk,1, 15, "Read ");
							COLOUR wattrset(paddisk,COLOR_PAIR(3));
							mvwprintw(paddisk,1, 20, "Write");
							COLOUR wattrset(paddisk,COLOR_PAIR(0));
							mvwprintw(paddisk,1, 25, "KB|0          |25         |50          |75       100|");
							break;
					}
				}
				if(disk_first_time) { 
					disk_first_time=0;
					mvwprintw(paddisk,2, 0, "Please wait - collecting disk data");
				} else {
					total_disk_read  = 0.0;
					total_disk_write = 0.0;
					total_disk_xfers = 0.0;
					disk_mb = 0;
					for (i = 0,k=0; i < disks; i++) {
						disk_read = DKDELTA(dk_rkb) / elapsed;
						disk_write = DKDELTA(dk_wkb) / elapsed;
						if((show_disk == SHOW_DISK_GRAPH) && (disk_read > 9999.9 || disk_write > 9999.9)) {
							disk_mb=1;
							COLOUR wattrset(paddisk, COLOR_PAIR(1));
							mvwprintw(paddisk,1, 25, "MB");
							COLOUR wattrset(paddisk, COLOR_PAIR(0));
							break;
						}
					}
					for (i = 0,k=0; i < disks; i++) {
						if(disk_only_mode && is_dgroup_name(p->dk[i].dk_name) == 0)
							continue;
						
						 //if(p->dk[i].dk_name[0] == 'h')
						 //continue;
						disk_busy = DKDELTA(dk_time) / elapsed;
						disk_read = DKDELTA(dk_rkb) / elapsed;
						disk_write = DKDELTA(dk_wkb) / elapsed;
						disk_xfers = DKDELTA(dk_xfers);
						
						total_disk_read  +=disk_read;
						total_disk_write +=disk_write;
						total_disk_xfers +=disk_xfers;
						
						if(disk_busy_peak[i] < disk_busy)
							disk_busy_peak[i] = disk_busy;
						if(disk_rate_peak[i] < (disk_read+disk_write))
							disk_rate_peak[i] = disk_read+disk_write;
						if(!show_all && disk_busy < 1)
							continue;
						
						if(strlen(p->dk[i].dk_name) > 8)
							str_p = &p->dk[i].dk_name[strlen(p->dk[i].dk_name) -8];
						else
							str_p = &p->dk[i].dk_name[0];
						
						if(show_disk == SHOW_DISK_STATS) {
							// output disks stats 
							mvwprintw(paddisk,2 + k, 0, "%-8s %3.0f%% %8.1f %8.1fKB/s %6.1f %5.1fKB  %3.0f%% %9.1fKB/s %3d",
									  str_p, 
									  disk_busy,
									  disk_read,
									  disk_write,
									  disk_xfers / elapsed,
									  disk_xfers == 0.0 ? 0.0 : 
									  (DKDELTA(dk_rkb) + DKDELTA(dk_wkb) ) / disk_xfers,
									  disk_busy_peak[i],
									  disk_rate_peak[i],
									  p->dk[i].dk_inflight);
							k++;
						}
						if(show_disk == SHOW_DISK_GRAPH) {
							// output disk bar graphs
							
							
							if(disk_mb) mvwprintw(paddisk,2 + k, 0, "%-8s %3.0f%% %6.1f %6.1f",
												  str_p, 
												  disk_busy,
												  disk_read/1024.0,
												  disk_write/1024.0);
							else mvwprintw(paddisk,2 + k, 0, "%-8s %3.0f%% %6.1f %6.1f",
										   str_p, 
										   disk_busy,
										   disk_read,
										   disk_write);
							mvwprintw(paddisk,2 + k, 27, "|                                                  ");
							wmove(paddisk,2 + k, 28);
							if(disk_busy >100) disk_busy=100;
							if( disk_busy > 0.0 && (disk_write+disk_read) > 0.1) {
								// 50 columns in the disk graph area so divide % by two 
								readers = disk_busy*disk_read/(disk_write+disk_read)/2;
								writers = disk_busy*disk_write/(disk_write+disk_read)/2;
								if(readers + writers > 50) {
									readers=0;
									writers=0;
								}
								// don't go beyond row 78 i.e. j = 28 + 50
								for (j = 0; j < readers && j<50; j++) {
									COLOUR wattrset(paddisk,COLOR_PAIR(12));
									wprintw(paddisk,"R");
									COLOUR wattrset(paddisk,COLOR_PAIR(0));
								}
								for (; j < readers + writers && j<50; j++) {
									COLOUR wattrset(paddisk,COLOR_PAIR(11));
									wprintw(paddisk,"W");
									COLOUR wattrset(paddisk,COLOR_PAIR(0));
								}
								for (j = disk_busy; j < 50; j++)
									wprintw(paddisk," ");
							} else {
								for (j = 0; j < 50; j++)
									wprintw(paddisk," ");
								if(p->dk[i].dk_time == 0.0) 
									mvwprintw(paddisk,2 + k, 27, "| disk busy not available");
							}
							if(disk_busy_peak[i] >100)
								disk_busy_peak[i]=100;
							
							mvwprintw(paddisk,2 + i, 77, "|");
							// check rounding has not got the peak ">" over the 100% 
							j = 28+(int)(disk_busy_peak[i]/2);
							if(j>77)
								j=77;
							mvwprintw(paddisk,2 + i, j, ">");
							k++;
						}
					}
					mvwprintw(paddisk,2 + k, 0, "Totals Read-MB/s=%-8.1f Writes-MB/s=%-8.1f Transfers/sec=%-8.1f",
							  total_disk_read  / 1024.0,
							  total_disk_write / 1024.0,
							  total_disk_xfers / elapsed);
					
				}
				DISPLAY(paddisk,3 + k);
*/
}

extern void uidiskgroup(WINDOW **winin, int *xin, int cols, int rows)
{
/*
				BANNER(paddg,"Disk-Group-I/O");
				if (dgroup_loaded != 2 || dgroup_total_disks == 0) {
					mvwprintw(paddg, 1, 1, "No Disk Groups found use -g groupfile when starting nmon");
					n = 0;
				} else if (disk_first_time) {
					disk_first_time=0;
					mvwprintw(paddg, 1, 1, "Please wait - collecting disk data");
				} else {
					mvwprintw(paddg, 1, 1, "Name          Disks AvgBusy Read-KB/s|Write  TotalMB/s   xfers/s BlockSizeKB");
					total_busy   = 0.0;
					total_rbytes = 0.0;
					total_wbytes = 0.0;
					total_xfers  = 0.0;
					for(k = n = 0; k < dgroup_total_groups; k++) {
						 //if (dgroup_name[k] == 0 )
						 //continue;
						disk_busy   = 0.0;
						disk_read = 0.0;
						disk_write = 0.0;
						disk_xfers  = 0.0;
						for (j = 0; j < dgroup_disks[k]; j++) {
							i = dgroup_data[k*DGROUPITEMS+j];
							if (i != -1) {
								disk_busy   += DKDELTA(dk_time) / elapsed;
								 //disk_read += DKDELTA(dk_reads) * p->dk[i].dk_bsize / 1024.0 /elapsed;
								 //disk_write += DKDELTA(dk_writes) * p->dk[i].dk_bsize / 1024.0 /elapsed;
								disk_read += DKDELTA(dk_rkb) /elapsed;
								disk_write += DKDELTA(dk_wkb) /elapsed;
								disk_xfers  += DKDELTA(dk_xfers) /elapsed;
							}
						}
						if (dgroup_disks[k] == 0)
							disk_busy = 0.0;
						else
							disk_busy = disk_busy / dgroup_disks[k];
						total_busy += disk_busy;
						total_rbytes += disk_read;
						total_wbytes += disk_write;
						total_xfers  += disk_xfers;
						//						if (!show_all && (disk_read < 1.0 && disk_write < 1.0))
						//continue;
						if ((disk_read + disk_write) == 0 || disk_xfers == 0)
							disk_size = 0.0;
						else
							disk_size = ((float)disk_read + (float)disk_write) / (float)disk_xfers;
						mvwprintw(paddg, n + 2, 1, "%-14s   %3d %5.1f%% %9.1f|%-9.1f %6.1f %9.1f %6.1f ",
								  dgroup_name[k], 
								  dgroup_disks[k],
								  disk_busy,
								  disk_read,
								  disk_write,
								  (disk_read + disk_write) / 1024, // in MB
								  disk_xfers,
								  disk_size
								  );
						n++;
					}
					mvwprintw(paddg, n + 2, 1, "Groups=%2d TOTALS %3d %5.1f%% %9.1f|%-9.1f %6.1f %9.1f",
							  n,
							  dgroup_total_disks,
							  total_busy / dgroup_total_disks,
							  total_rbytes,
							  total_wbytes,
							  (((double)total_rbytes + (double)total_wbytes)) / 1024, // in MB
							  total_xfers
							  );
				}
				DISPLAY(paddg, 3 + dgroup_total_groups);
*/
}

extern void uidiskmap(WINDOW **winin, int *xin, int cols, int rows)
{
/*
// for Disk Busy rain style output covering 100's of diskss on one screen
const char disk_busy_map_ch[] =
"_____.....----------++++++++++oooooooooo0000000000OOOOOOOOOO8888888888XXXXXXXXXX##########@@@@@@@@@@*";
				BANNER(padmap,"Disk %%Busy Map");
				mvwprintw(padmap,0, 18,"Key: @=90 #=80 X=70 8=60 O=50 0=40 o=30 +=20 -=10 .=5 _=0%%");
				mvwprintw(padmap,1, 0,"             Disk No.  1         2         3         4         5         6   ");
				if(disk_first_time) {
					disk_first_time=0;
					mvwprintw(padmap,2, 0,"Please wait - collecting disk data");
				} else {
					mvwprintw(padmap,2, 0,"Disks=%-4d   0123456789012345678901234567890123456789012345678901234567890123", disks);
					mvwprintw(padmap,3, 0,"disk 0 to 63 ");
					for (i = 0; i < disks; i++) {
						disk_busy = DKDELTA(dk_time) / elapsed;
						disk_read = DKDELTA(dk_rkb) / elapsed;
						disk_write = DKDELTA(dk_wkb) / elapsed;
						// ensure boundaries
						if (disk_busy <  0)
							disk_busy=0;
						else
							if (disk_busy > 99) disk_busy=99;
						
#define MAPWRAP 64
						mvwprintw(padmap,3 + (int)(i/MAPWRAP), 13+ (i%MAPWRAP), "%c",disk_busy_map_ch[(int)disk_busy]);
					}
				}
				DISPLAY(padmap,4 + disks/MAPWRAP);
*/
}

extern void uifilesys(WINDOW **winin, int *xin, int cols, int rows)
{
/*
#ifdef JFS
				BANNER(padjfs,"Filesystems");
				mvwprintw(padjfs,1, 0, "Filesystem            SizeMB  FreeMB  Use%% Type     MountPoint");
				
				for (k = 0; k < jfses; k++) {
					fs_size=0;
					fs_bsize=0;
					fs_free=0;
					fs_size_used=100.0;
					if(jfs[k].mounted) {
						if(!strncmp(jfs[k].name,"/Users/stollcri/Documents/code/c/nmond/dbg/",6)       // sub directorys have to be fake too 
						   || !strncmp(jfs[k].name,"/sys/",5)
						   || !strncmp(jfs[k].name,"/dev/",5)
						   || !strncmp(jfs[k].name,"/proc",6) // one more than the string to ensure the NULL 
						   || !strncmp(jfs[k].name,"/sys",5)
						   || !strncmp(jfs[k].name,"/dev",5)
						   || !strncmp(jfs[k].name,"/rpc_pipe",10)
						   ) { // /proc gives invalid/insane values 
							mvwprintw(padjfs,2+k, 0, "%-14s", jfs[k].name);
							mvwprintw(padjfs,2+k, 27, "-");
							mvwprintw(padjfs,2+k, 35, "-");
							mvwprintw(padjfs,2+k, 41, "-");
							mvwprintw(padjfs,2+k, 43, "%-8s not a real filesystem",jfs[k].type);
						} else {
							statfs_buffer.f_blocks=0;
							if((ret=fstatfs( jfs[k].fd, &statfs_buffer)) != -1) {
								if(statfs_buffer.f_blocks != 0) {
									// older Linux seemed to always report in 4KB blocks but
									 newer Linux release use the f_bsize block sizes but
									 the man statfs docs the field as the natural I/O size so
									 the blocks reported here are ambigous in size 
									if(statfs_buffer.f_bsize == 0) 
										fs_bsize = 4.0 * 1024.0;
									else
										fs_bsize = statfs_buffer.f_bsize;
									// convert blocks to MB 
									fs_size = (float)statfs_buffer.f_blocks * fs_bsize/1024.0/1024.0;
									
									// find the best size info available f_bavail is like df reports
									 otherwise use f_bsize (this includes inode blocks) 
									if(statfs_buffer.f_bavail == 0) 
										fs_free = (float)statfs_buffer.f_bfree  * fs_bsize/1024.0/1024.0;
									else
										fs_free = (float)statfs_buffer.f_bavail  * fs_bsize/1024.0/1024.0;
									
									// this is a percentage 
									fs_size_used = (fs_size - (float)statfs_buffer.f_bfree  * fs_bsize/1024.0/1024.0)/fs_size * 100.0;
									// try to get the same number as df using kludge 
									//fs_size_used += 1.0; 
									if (fs_size_used >100.0)
										fs_size_used = 100.0;
									
									if( (i=strlen(jfs[k].device)) <20)
										str_p=&jfs[k].device[0];
									else {
										str_p=&jfs[k].device[i-20];
									}
									mvwprintw(padjfs,2+k, 0, "%-20s %7.0f %7.0f %4.0f%% %-8s %s",
											  str_p,
											  fs_size,
											  fs_free,
											  ceil(fs_size_used),
											  jfs[k].type,
											  jfs[k].name
											  );
									
								} else {
									mvwprintw(padjfs,2+k, 0, "%s", jfs[k].name);
									mvwprintw(padjfs,2+k, 43, "%-8s size=zero blocks!", jfs[k].type);
								}
							}
							else {
								mvwprintw(padjfs,2+k, 0, "%s", jfs[k].name);
								mvwprintw(padjfs,2+k, 43, "%-8s statfs failed", jfs[k].type);
							}
						}
					} else {
						mvwprintw(padjfs,2+k, 0, "%-14s", jfs[k].name);
						mvwprintw(padjfs,2+k, 43, "%-8s not mounted",jfs[k].type);
					}
				}
				DISPLAY(padjfs,2 + jfses);
#endif // JFS
*/
}

extern void uikernel(WINDOW **winin, int *xin, int cols, int rows)
{
/*
				proc_read(P_UPTIME);
				proc_read(P_LOADAVG);
				proc_kernel();
				BANNER(padkstat,"Kernel Stats");
				mvwprintw(padkstat,1, 1, "RunQueue       %8lld   Load Average    CPU use since boot time",
						  p->cpu_total.running);
				updays=p->cpu_total.uptime/60/60/24;
				uphours=(p->cpu_total.uptime-updays*60*60*24)/60/60;
				upmins=(p->cpu_total.uptime-updays*60*60*24-uphours*60*60)/60;
				mvwprintw(padkstat,2, 1, "ContextSwitch  %8.1f    1 mins %5.2f    Uptime Days=%3d Hours=%2d Mins=%2d",
						  (float)(p->cpu_total.ctxt - q->cpu_total.ctxt)/elapsed,
						  (float)p->cpu_total.mins1,
						  updays, uphours, upmins);
				updays=p->cpu_total.idletime/60/60/24;
				uphours=(p->cpu_total.idletime-updays*60*60*24)/60/60;
				upmins=(p->cpu_total.idletime-updays*60*60*24-uphours*60*60)/60;
				mvwprintw(padkstat,3, 1, "Forks          %8.1f    5 mins %5.2f    Idle   Days=%3d Hours=%2d Mins=%2d",
						  (float)(p->cpu_total.procs - q->cpu_total.procs)/elapsed,
						  (float)p->cpu_total.mins5,
						  updays, uphours, upmins);
				
				mvwprintw(padkstat,4, 1, "Interrupts     %8.1f   15 mins %5.2f",
						  (float)(p->cpu_total.intr - q->cpu_total.intr)/elapsed,
						  (float)p->cpu_total.mins15);
				average = (p->cpu_total.uptime - p->cpu_total.idletime)/ p->cpu_total.uptime *100.0;
				if( average > 0.0)
					mvwprintw(padkstat,4, 46, "Average CPU use=%6.2f%%", average);
				else
					mvwprintw(padkstat,4, 46, "Uptime has overflowed");
				DISPLAY(padkstat,5);
*/
}

extern void uimemory(WINDOW **winin, int *xin, int cols, int rows)
{
/*
				proc_read(P_MEMINFO);
				proc_mem();
				BANNER(padmem,"Memory Stats");
				mvwprintw(padmem,1, 1, "               RAM     High      Low     Swap    Page Size=%d KB",pagesize/1024);
				mvwprintw(padmem,2, 1, "Total MB    %8.1f %8.1f %8.1f %8.1f ",
						  p->mem.memtotal/1024.0,
						  p->mem.hightotal/1024.0,
						  p->mem.lowtotal/1024.0,
						  p->mem.swaptotal/1024.0);
				mvwprintw(padmem,3, 1, "Free  MB    %8.1f %8.1f %8.1f %8.1f ",
						  p->mem.memfree/1024.0,
						  p->mem.highfree/1024.0,
						  p->mem.lowfree/1024.0,
						  p->mem.swapfree/1024.0);
				mvwprintw(padmem,4, 1, "Free Percent %7.1f%% %7.1f%% %7.1f%% %7.1f%% ",
						  p->mem.memfree  == 0 ? 0.0 : 100.0*(float)p->mem.memfree/(float)p->mem.memtotal,
						  p->mem.highfree == 0 ? 0.0 : 100.0*(float)p->mem.highfree/(float)p->mem.hightotal,
						  p->mem.lowfree  == 0 ? 0.0 : 100.0*(float)p->mem.lowfree/(float)p->mem.lowtotal,
						  p->mem.swapfree == 0 ? 0.0 : 100.0*(float)p->mem.swapfree/(float)p->mem.swaptotal);
				
				
				mvwprintw(padmem,5, 1, "            MB                  MB                  MB");
#ifdef LARGEMEM
				mvwprintw(padmem,6, 1, "                     Cached=%8.1f     Active=%8.1f",
						  p->mem.cached/1024.0,
						  p->mem.active/1024.0);
#else
				mvwprintw(padmem,6, 1, " Shared=%8.1f     Cached=%8.1f     Active=%8.1f",
						  p->mem.memshared/1024.0,
						  p->mem.cached/1024.0,
						  p->mem.active/1024.0);
				mvwprintw(padmem,5, 68, "MB");
				mvwprintw(padmem,6, 55, "bigfree=%8.1f",
						  p->mem.bigfree/1024);
#endif //LARGEMEM
				mvwprintw(padmem,7, 1, "Buffers=%8.1f Swapcached=%8.1f  Inactive =%8.1f",
						  p->mem.buffers/1024.0,
						  p->mem.swapcached/1024.0,
						  p->mem.inactive/1024.0);
				
				mvwprintw(padmem,8, 1, "Dirty  =%8.1f Writeback =%8.1f  Mapped   =%8.1f",
						  p->mem.dirty/1024.0,
						  p->mem.writeback/1024.0,
						  p->mem.mapped/1024.0);
				mvwprintw(padmem,9, 1, "Slab   =%8.1f Commit_AS =%8.1f PageTables=%8.1f",
						  p->mem.slab/1024.0,
						  p->mem.committed_as/1024.0,
						  p->mem.pagetables/1024.0);

				DISPLAY(padmem,10);
*/
}

extern void uimemlarge(WINDOW **winin, int *xin, int cols, int rows)
{
/*
				proc_read(P_MEMINFO);
				proc_mem();
				BANNER(padlarge,"Large (Huge) Page Stats");
				if(p->mem.hugetotal > 0) {
					if(p->mem.hugetotal - p->mem.hugefree > huge_peak)
						huge_peak = p->mem.hugetotal - p->mem.hugefree;
					mvwprintw(padlarge,1, 1, "Total Pages=%7ld   100.0%%   Huge Page Size =%ld KB",    p->mem.hugetotal, p->mem.hugesize);
					mvwprintw(padlarge,2, 1, "Used  Pages=%7ld   %5.1f%%   Used Pages Peak=%-8ld",
							  (long)(p->mem.hugetotal - p->mem.hugefree),
							  (p->mem.hugetotal - p->mem.hugefree)/(float)p->mem.hugetotal*100.0,
							  huge_peak);
					mvwprintw(padlarge,3, 1, "Free  Pages=%7ld   %5.1f%%",    p->mem.hugefree, p->mem.hugefree/(float)p->mem.hugetotal*100.0);
				} else {
					mvwprintw(padlarge,1, 1, " There are no Huge Pages");
					mvwprintw(padlarge,2, 1, " - see /proc/meminfo");
				}
				DISPLAY(padlarge,4);
*/
}

extern void uimemvirtual(WINDOW **winin, int *xin, int cols, int rows)
{
/*
#define VMDELTA(variable) (p->vm.variable - q->vm.variable)
#define VMCOUNT(variable) (p->vm.variable                 )
				ret = read_vmstat();
				BANNER(padpage,"Virtual-Memory");
				if(ret < 0 ) {
					mvwprintw(padpage,2, 2, "Virtual Memory stats not supported with this kernel");
					mvwprintw(padpage,3, 2, "/proc/vmstat only seems to appear in 2.6 onwards");
					
				} else {
					if(vm_first_time) {
						mvwprintw(padpage,2, 2, "Please wait - collecting data");
						vm_first_time=0;
					} else {
						mvwprintw(padpage,1, 0, "nr_dirty    =%9lld pgpgin      =%8lld",
								  VMCOUNT(nr_dirty),
								  VMDELTA(pgpgin));
						mvwprintw(padpage,2, 0, "nr_writeback=%9lld pgpgout     =%8lld",
								  VMCOUNT(nr_writeback),
								  VMDELTA(pgpgout));
						mvwprintw(padpage,3, 0, "nr_unstable =%9lld pgpswpin    =%8lld",
								  VMCOUNT(nr_unstable),
								  VMDELTA(pswpin));
						mvwprintw(padpage,4, 0, "nr_table_pgs=%9lld pgpswpout   =%8lld",
								  VMCOUNT(nr_page_table_pages),
								  VMDELTA(pswpout));
						mvwprintw(padpage,5, 0, "nr_mapped   =%9lld pgfree      =%8lld",
								  VMCOUNT(nr_mapped),
								  VMDELTA(pgfree));
						mvwprintw(padpage,6, 0, "nr_slab     =%9lld pgactivate  =%8lld",
								  VMCOUNT(nr_slab),
								  VMDELTA(pgactivate));
						mvwprintw(padpage,7, 0, "                       pgdeactivate=%8lld",
								  VMDELTA(pgdeactivate));
						mvwprintw(padpage,8, 0, "allocstall  =%9lld pgfault     =%8lld  kswapd_steal     =%7lld",
								  VMDELTA(allocstall),
								  VMDELTA(pgfault),
								  VMDELTA(kswapd_steal));
						mvwprintw(padpage,9, 0, "pageoutrun  =%9lld pgmajfault  =%8lld  kswapd_inodesteal=%7lld",
								  VMDELTA(pageoutrun),
								  VMDELTA(pgmajfault),
								  VMDELTA(kswapd_inodesteal));
						mvwprintw(padpage,10, 0,"slabs_scanned=%8lld pgrotated   =%8lld  pginodesteal     =%7lld",
								  VMDELTA(slabs_scanned),
								  VMDELTA(pgrotated),
								  VMDELTA(pginodesteal));
						
						
						
						mvwprintw(padpage,1, 46, "              High Normal    DMA");
						mvwprintw(padpage,2, 46, "alloc      %7lld%7lld%7lld",
								  VMDELTA(pgalloc_high),
								  VMDELTA(pgalloc_normal),
								  VMDELTA(pgalloc_dma));
						mvwprintw(padpage,3, 46, "refill     %7lld%7lld%7lld",
								  VMDELTA(pgrefill_high),
								  VMDELTA(pgrefill_normal),
								  VMDELTA(pgrefill_dma));
						mvwprintw(padpage,4, 46, "steal      %7lld%7lld%7lld",
								  VMDELTA(pgsteal_high),
								  VMDELTA(pgsteal_normal),
								  VMDELTA(pgsteal_dma));
						mvwprintw(padpage,5, 46, "scan_kswapd%7lld%7lld%7lld",
								  VMDELTA(pgscan_kswapd_high),
								  VMDELTA(pgscan_kswapd_normal),
								  VMDELTA(pgscan_kswapd_dma));
						mvwprintw(padpage,6, 46, "scan_direct%7lld%7lld%7lld",
								  VMDELTA(pgscan_direct_high),
								  VMDELTA(pgscan_direct_normal),
								  VMDELTA(pgscan_direct_dma));
					}
				}
				DISPLAY(padpage,11);
*/
}

extern void uineterrors(WINDOW **winin, int *xin, int cols, int rows)
{
/*
				BANNER(padneterr,"Network Error Counters");
				mvwprintw(padneterr,1, 0, "I/F Name iErrors iDrop iOverrun iFrame oErrors   oDrop oOverrun oCarrier oColls ");
				for (i = 0; i < networks; i++) {
					mvwprintw(padneterr,2 + i, 0, "%8s %7lu %7lu %7lu %7lu %7lu %7lu %7lu %7lu %7lu",
									&p->ifnets[i].if_name[0],
									p->ifnets[i].if_ierrs,   
									p->ifnets[i].if_idrop,   
									p->ifnets[i].if_ififo,   
									p->ifnets[i].if_iframe,   
									p->ifnets[i].if_oerrs,   
									p->ifnets[i].if_odrop,   
									p->ifnets[i].if_ofifo,   
									p->ifnets[i].if_ocarrier,   
									p->ifnets[i].if_ocolls);   
					
				}
				DISPLAY(padneterr,networks + 2);
				if(show_neterror > 0) show_neterror--;
*/
}

extern void uinetfilesys(WINDOW **winin, int *xin, int cols, int rows)
{
/*
				proc_read(P_NFS);
				proc_read(P_NFSD);
				proc_nfs();
				
				if(nfs_first_time) {
					memcpy(&q->nfs,&p->nfs,sizeof(struct nfs_stat));
					nfs_first_time=0;
				}
				if(nfs_clear) {
					nfs_clear=0;
					for(i=0;i<25;i++)
						mvwprintw(padnfs,i, 0, "                                                                                ");
				}
				BANNER(padnfs,"Network Filesystem (NFS) I/O Operations per second");
				if(show_nfs == 1) {
					if(nfs_v2c_found || nfs_v2s_found) 
						mvwprintw(padnfs,1, 0, " Version 2        Client   Server");
					else
						mvwprintw(padnfs,1, 0, " Version 2 not active");
					
					if(nfs_v3c_found || nfs_v3s_found)
						mvwprintw(padnfs,1, 41, "Version 3      Client   Server");
					else
						mvwprintw(padnfs,1, 41, " Version 3 not active");
				} 
				if(show_nfs == 2 ) {
					if(nfs_v4c_found) 
						mvwprintw(padnfs,1, 0, " Version 4 Client (%d Stats found)", nfs_v4c_names_count);
					else
						mvwprintw(padnfs,1, 0, " Version 4 Client side not active");
				} 
				if(show_nfs == 3 ) {
					if(nfs_v4s_found)
						mvwprintw(padnfs,1, 0, " Version 4 Server (%d Stats found)", nfs_v4s_names_count);
					else
						mvwprintw(padnfs,1, 0, " Version 4 Server side not active");
				}
#define NFS_TOTAL(member) (double)(p->member)
#define NFS_DELTA(member) (((double)(p->member - q->member)/elapsed))
				v2c_total =0;
				v2s_total =0;
				v3c_total =0;
				v3s_total =0;
				v4c_total =0;
				v4s_total =0;
				if(nfs_v3c_found || nfs_v3s_found) {
					for(i=0;i<18;i++) {	// NFS V2 Client & Server 
						if(show_nfs == 1) 
							mvwprintw(padnfs,2+i,  3, "%12s %8.1f %8.1f",
									  nfs_v2_names[i],
									  NFS_DELTA(nfs.v2c[i]),
									  NFS_DELTA(nfs.v2s[i]));
						v2c_total +=NFS_DELTA(nfs.v2c[i]);
						v2s_total +=NFS_DELTA(nfs.v2s[i]);
					}
				}
				if(nfs_v3c_found || nfs_v3s_found) {
					for(i=0;i<22;i++) {	// NFS V3 Client & Server 
						if(show_nfs == 1)
							mvwprintw(padnfs,2+i, 41, "%12s %8.1f %8.1f",
									  nfs_v3_names[i],
									  NFS_DELTA(nfs.v3c[i]),
									  NFS_DELTA(nfs.v3s[i]));
						v3c_total +=NFS_DELTA(nfs.v3c[i]);
						v3s_total +=NFS_DELTA(nfs.v3s[i]);
					}
				}
				
				if(nfs_v4c_found) {
					for(i=0;i<18;i++) {	// NFS V4 Client 
						if(show_nfs == 2) {
							mvwprintw(padnfs,2+i, 0, "%12s%7.1f",
									  nfs_v4c_names[i],
									  NFS_DELTA(nfs.v4c[i]));
						}
						v4c_total +=NFS_DELTA(nfs.v4c[i]);
					}
					for(i=18;i<35;i++) {	// NFS V4 Client 
						if(show_nfs == 2) {
							mvwprintw(padnfs,2+i-18, 20, "%12s%7.1f",
									  nfs_v4c_names[i],
									  NFS_DELTA(nfs.v4c[i]));
						}
						v4c_total +=NFS_DELTA(nfs.v4c[i]);
					}
				}
				
				if(nfs_v4s_found) {
					for(i=0;i<18;i++) {	// NFS V4 Server 
						if(show_nfs == 3) {
							mvwprintw(padnfs,2+i, 0, "%12s%7.1f",
									  nfs_v4s_names[i],
									  NFS_DELTA(nfs.v4s[i]));
						}
						v4s_total +=NFS_DELTA(nfs.v4s[i]);
					}
					for(i=18;i<36;i++) {	// NFS V4 Server 
						if(show_nfs == 3) {
							mvwprintw(padnfs,2+i-18, 19, "%12s%7.1f",
									  nfs_v4s_names[i],
									  NFS_DELTA(nfs.v4s[i]));
						}
						v4s_total +=NFS_DELTA(nfs.v4s[i]);
					}
					for(i=36;i<54 && i<nfs_v4s_names_count;i++) {	// NFS V4 Server 
						if(show_nfs == 3) {
							mvwprintw(padnfs,2+i-36, 39, "%12s%7.1f",
									  nfs_v4s_names[i],
									  NFS_DELTA(nfs.v4s[i]));
						}
						v4s_total +=NFS_DELTA(nfs.v4s[i]);
					}
					for(i=54;i<=70 && i<nfs_v4s_names_count;i++) {	// NFS V4 Server 
						if(show_nfs == 3) {
							mvwprintw(padnfs,2+i-54, 59, "%12s%7.1f",
									  nfs_v4s_names[i],
									  NFS_DELTA(nfs.v4s[i]));
						}
						v4s_total +=NFS_DELTA(nfs.v4s[i]);
					}
				}
				mvwprintw(padnfs,2+18,  1, "--NFS-Totals->---Client----Server--");
				// if(nfs_v2c_found || nfs_v2s_found) 
				mvwprintw(padnfs,2+19,  1, "NFSv2 Totals->%9.1f %9.1f", v2c_total,v2s_total);
				// if(nfs_v3c_found || nfs_v3s_found)
				mvwprintw(padnfs,2+20,  1, "NFSv3 Totals->%9.1f %9.1f", v3c_total,v3s_total);
				// if(nfs_v4c_found || nfs_v4s_found)
				mvwprintw(padnfs,2+21,  1, "NFSv4 Totals->%9.1f %9.1f", v4c_total,v4s_total);
				
				DISPLAY(padnfs,24);
*/
}

extern void uinetwork(WINDOW **winin, int *xin, int cols, int rows)
{
/*
				BANNER(padnet,"Network I/O");
				mvwprintw(padnet,1, 0, "I/F Name Recv=KB/s Trans=KB/s packin packout insize outsize Peak->Recv Trans");
				proc_net();
				for (i = 0; i < networks; i++) {
					
#define IFDELTA(member) ((float)( (q->ifnets[i].member > p->ifnets[i].member) ? 0 : (p->ifnets[i].member - q->ifnets[i].member)/elapsed) )
#define IFDELTA_ZERO(member1,member2) ((IFDELTA(member1) == 0) || (IFDELTA(member2)== 0)? 0.0 : IFDELTA(member1)/IFDELTA(member2) )
					
					if(net_read_peak[i] < IFDELTA(if_ibytes) / 1024.0)
						net_read_peak[i] = IFDELTA(if_ibytes) / 1024.0;
					if(net_write_peak[i] < IFDELTA(if_obytes) / 1024.0)
						net_write_peak[i] = IFDELTA(if_obytes) / 1024.0;
					
					mvwprintw(padnet,2 + i, 0, "%8s %7.1f %7.1f    %6.1f   %6.1f  %6.1f %6.1f    %7.1f %7.1f   ",
									&p->ifnets[i].if_name[0],
									IFDELTA(if_ibytes) / 1024.0,   
									IFDELTA(if_obytes) / 1024.0, 
									IFDELTA(if_ipackets), 
									IFDELTA(if_opackets),
									IFDELTA_ZERO(if_ibytes, if_ipackets),
									IFDELTA_ZERO(if_obytes, if_opackets),
									net_read_peak[i],
									net_write_peak[i]
									);
				}
				DISPLAY(padnet,networks + 2);
*/
}

void uisys(WINDOW **winin, int *xin, int cols, int rows, struct syshw hw, struct syskern kern)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}

	uibanner(win, cols, "About This Mac");
	mvwprintw(win, 1, 2, "%s", hw.model);
	mvwprintw(win, 2, 2, "%s %s", hw.cpuvendor, hw.cpubrand);
	mvwprintw(win, 3, 2, "%s", kern.version);
	mvwprintw(win, 4, 2, "OS Release: %s / OS Version: %s", kern.osrelease, kern.osversion);
	mvwprintw(win, 5, 2, "CPUs: %d (%d cores, %d physical, %d logical)", hw.cpucount, kern.corecount, hw.physicalcpucount, hw.logicalcpucount);
	mvwprintw(win, 6, 2, "Memory: %4.2f GB (%4.2f GB non-kernel in use)", bytestogb64(hw.memorysize), bytestogb(hw.usermemory));
	
	mvwprintw(win, 8, 2, "Domain   : %s", kern.domainname);
	mvwprintw(win, 9, 2, "Booted   : %s", kern.boottimestring);
	uidisplay(win, xin, cols, 10, rows);
}

static int comparepercentasc(const void *val1, const void *val2)
{
	struct sysproc *percent1 = (struct sysproc *)val1;
	struct sysproc *percent2 = (struct sysproc *)val2;

	// return (int)(percent1->percentage - percent2->percentage);
	if (percent1->percentage < percent2->percentage) {
		return -1;
	} else if (percent1->percentage > percent2->percentage) {
		return 1;
	} else {
		return 0;
	}
}

static int comparepercentdes(const void *val1, const void *val2)
{
	struct sysproc *percent1 = (struct sysproc *)val1;
	struct sysproc *percent2 = (struct sysproc *)val2;

	if (percent1->percentage > percent2->percentage) {
		return -1;
	} else if (percent1->percentage < percent2->percentage) {
		return 1;
	} else {
		return 0;
	}
}

void uitop(WINDOW **winin, int *xin, int cols, int rows, struct sysproc *procs, int processcount, int topmode, bool updateddata)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}

	int procstoshow = processcount;
	if(procstoshow > rows) {
		procstoshow = rows - 4;
	}

	wmove(win, 1, 1);
	wclrtobot(win);

	// TODO: this is not working???
	uibanner(win, cols, "Top Processes");
	if(updateddata) {
		heapsort(procs, processcount, sizeof(struct sysproc), comparepercentdes);
	}

	switch(topmode) {
		case TOP_MODE_A:
			mvwprintw(win, 1, 1, "ID      NAME                   %%CPU     MEM      PHYS   PGRP   PPID   STATE");
			break;
		case TOP_MODE_B:
			mvwprintw(win, 1, 1, "PID     %%CPU  RESSIZE   COMMAND                                            ");
			break;
	}

	char *statustext = malloc(6);
	for (int i = 0; i < procstoshow; i++) {


		switch(procs[i].status){
			case SIDL:
				statustext = "IDLE";
				break;
			case SRUN:
				statustext = "RUN";
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
				mvwprintw(win, (i + 2), 1, "%-7d %-22.22s %4.1f %9s %9s %-6d %-7d %-5.5s", 
					procs[i].pid,
					procs[i].name,
					procs[i].percentage,
					uireadablebytes(procs[i].residentmem),
					uireadablebytes(procs[i].physicalmem),
					procs[i].pgid,
					procs[i].parentpid,
					statustext
					);
				break;
			case TOP_MODE_B:
				mvwprintw(win, (i + 2), 1, "%-7d %4.1f %9s  %-53.53s", 
					procs[i].pid,
					procs[i].percentage,
					uireadablebytes(procs[i].residentmem),
					procs[i].path
					);
				break;
		}
	}
	uidisplay(win, xin, cols, 27, rows);
	

	//mvwprintw(winin,1, 1, "  PID      PPID  Pgrp Nice Prior Status    proc-Flag Command");
	//"  PID    %%CPU ResSize    Command                                            ";
	//
	//DISPLAY(winin,3 + j);
/*
				// Get the details of the running processes 
				skipped = 0;
				n = getprocs(0);
				if (n > p->nprocs) {
					n = n +128; // allow for growth in the number of processes in the mean time 
					p->procs = REALLOC(p->procs, sizeof(struct procsinfo ) * (n+1) ); // add one to avoid overrun 
					p->nprocs = n;
				}
				
				n = getprocs(1);
				
				if (topper_size < n) {
					topper = REALLOC(topper, sizeof(struct topper ) * (n+1) ); // add one to avoid overrun 
					topper_size = n;
				}

				switch(show_topmode) {
					default:
					case 3: qsort((void *) & topper[0], max_sorted, sizeof(struct topper ), &cpu_compare );
						break;
					case 4: qsort((void *) & topper[0], max_sorted, sizeof(struct topper ), &size_compare );
						break;
					case 5: qsort((void *) & topper[0], max_sorted, sizeof(struct topper ), &disk_compare );
						break;
				}
				//BANNER(winin,"Top Processes");
				if(isroot) {
					mvwprintw(winin,0, 15, "Procs=%d mode=%d (1=Basic, 3=Perf 4=Size 5=I/O)", n, show_topmode);
				} else {
					mvwprintw(winin,0, 15, "Procs=%d mode=%d (1=Basic, 3=Perf 4=Size 5=(root-only))", n, show_topmode);
				}
				if(top_first_time) {
					top_first_time = 0;
					mvwprintw(winin,1, 1, "Please wait - information being collected");
				} else {
					switch (show_topmode) {
						case 1:
							mvwprintw(winin,1, 1, "  PID      PPID  Pgrp Nice Prior Status    proc-Flag Command");
							for (j = 0; j < max_sorted; j++) {
								i = topper[j].index;
								if (p->procs[i].pi_pgrp == p->procs[i].pi_pid)
									strcpy(pgrp, "none");
								else
									sprintf(&pgrp[0], "%d", p->procs[i].pi_pgrp);
								// skip over processes with 0 CPU 
								if(!show_all && (topper[j].time/elapsed < ignore_procdisk_threshold) && !cmdfound) 
									break;
								if( x + j + 2 - skipped > LINES+2) // +2 to for safety :-) 
									break;
								mvwprintw(winin,j + 2 - skipped, 1, "%7d %7d %6s %4d %4d %9s 0x%08x %1s %-32s",
												p->procs[i].pi_pid,
												p->procs[i].pi_ppid,
												pgrp,
												p->procs[i].pi_nice,
												p->procs[i].pi_pri,
												
												(topper[j].time * 100 / elapsed) ? "Running "
												: get_state(p->procs[i].pi_state),
												p->procs[i].pi_flags,
												(p->procs[i].pi_tty_nr ? "F" : " "),
												p->procs[i].pi_comm);
							}
							break;
						case 3:
						case 4:
						case 5:
							
							if(show_args == ARGS_ONLY)  {
								formatstring = "  PID    %%CPU ResSize    Command                                            ";
							} else if(COLS > 119) {
								if(show_topmode == 5)
									formatstring = "  PID       %%CPU    Size     Res    Res     Res     Res    Shared   StorageKB Command";
								else
									formatstring = "  PID       %%CPU    Size     Res    Res     Res     Res    Shared    Faults   Command";
							} else {
								if(show_topmode == 5)
									formatstring = "  PID    %%CPU  Size   Res   Res   Res   Res Shared StorageKB Command";
								else
									formatstring = "  PID    %%CPU  Size   Res   Res   Res   Res Shared   Faults  Command";
							}
							mvwprintw(winin,1, y, formatstring);
							
							if(show_args == ARGS_ONLY) {
								formatstring = "         Used      KB                                                        ";
							} else if(COLS > 119) {
								if(show_topmode == 5)
									formatstring = "            Used      KB     Set    Text    Data     Lib    KB    Read Write";
								else
									formatstring = "            Used      KB     Set    Text    Data     Lib    KB     Min   Maj";
							} else {
								if(show_topmode == 5)
									formatstring = "         Used    KB   Set  Text  Data   Lib    KB ReadWrite ";
								else
									formatstring = "         Used    KB   Set  Text  Data   Lib    KB  Min  Maj ";
							}
							mvwprintw(winin,2, 1, formatstring);
							for (j = 0; j < max_sorted; j++) {
								i = topper[j].index;
								if(!show_all) { 
									// skip processes with zero CPU/io 
									if(show_topmode == 3 && (topper[j].time/elapsed) < ignore_procdisk_threshold && !cmdfound)
										break;
									if(show_topmode == 5 && (topper[j].io < ignore_io_threshold && !cmdfound))
										break;
								}
								if( x + j + 3 - skipped > LINES+2) // +2 to for safety :-) XYZXYZ
									break;
								if(cmdfound && !cmdcheck(p->procs[i].pi_comm)) {
									skipped++;
									continue;
								}
								if(show_args == ARGS_ONLY){
									mvwprintw(winin,j + 3 - skipped, 1, 
											  "%7d %5.1f %7lu %-120s",
											  p->procs[i].pi_pid,
											  topper[j].time / elapsed,
											  p->procs[i].statm_resident*pagesize/1024, // in KB 
											  args_lookup(p->procs[i].pi_pid,
														  p->procs[i].pi_comm));
								}
								else {
									if(COLS > 119)
										formatstring = "%8d %7.1f %7lu %7lu %7lu %7lu %7lu %5lu %6d %6d %-32s";
									else
										formatstring = "%7d %5.1f %5lu %5lu %5lu %5lu %5lu %5lu %4d %4d %-32s";
									
									mvwprintw(winin,j + 3 - skipped, 1, formatstring,
											  p->procs[i].pi_pid,
											  topper[j].time/elapsed,
											  // topper[j].time /1000.0 / elapsed,
											  p->procs[i].statm_size*pagesize/1024UL, // in KB 
											  p->procs[i].statm_resident*pagesize/1024UL, // in KB 
											  p->procs[i].statm_trs*pagesize/1024UL, // in KB 
											  p->procs[i].statm_drs*pagesize/1024UL, // in KB 
											  p->procs[i].statm_lrs*pagesize/1024UL, // in KB 
											  p->procs[i].statm_share*pagesize/1024UL, // in KB 
											  show_topmode == 5 ? (int)(COUNTDELTA(read_io)  / elapsed / 1024) : (int)(COUNTDELTA(pi_minflt) / elapsed),
											  show_topmode == 5 ? (int)(COUNTDELTA(write_io) / elapsed / 1024) : (int)(COUNTDELTA(pi_majflt) / elapsed),
											  p->procs[i].pi_comm);
								}
							}
							break;
					}
				}
				DISPLAY(winin,3 + j);
*/
}

void uiwarn(WINDOW **winin, int *xin, int cols, int rows)
{
	WINDOW *win = *winin;
	if (win == NULL) {
		return;
	}

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
	uidisplay(win, xin, cols, 4, rows);
}
