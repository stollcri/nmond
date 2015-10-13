/**
 * nmond.h -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
 *
 * 
 * nmond -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
 *  https://github.com/stollcri/nmond
 *  forked from (near complete rewrite of):
 *   lmon.c -- Curses based Performance Monitor for Linux
 *   Developer: Nigel Griffiths.
 *   (lmon15g.c dated 2015-07-13)
 *
 * 
 * Copyright (c) 2009-2015, Nigel Griffiths
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

#include "nmond.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "pidhash.h"
#include "sysinfo.h"
#include "uicli.h"
#include "uicurses.h"

static inline void exitapp() __attribute__ ((noreturn));
static inline void exitapp()
{
	nocbreak();
	endwin();
	exit(0);
}

static inline void handleinterupt(int signum)
{
	// window size change
	// TODO: clean up
	if (signum == SIGWINCH) {
		endwin(); // stop + start curses so it works out the # of row and cols
		initscr();
		cbreak();
		signal(SIGWINCH, handleinterupt);
		// COLOUR color = has_colors();
		// COLOUR start_color();
		// COLOUR init_pairs();
		clear();
		return;
	// all other interupts
	} else {
		exitapp();
	}
}

static inline void setinterupthandlers()
{
	signal(SIGUSR1, handleinterupt);
	signal(SIGUSR2, handleinterupt);
	signal(SIGINT, handleinterupt);
	signal(SIGWINCH, handleinterupt);
}

static void setwinstate(struct uiwins *wins, struct nmondstate *state, int input)
{
	switch (input) {
		case 'a':
			break;
		case 'A':
			break;
		case 'b':
			state->color = state->color ? 0 : 1;
			break;
		case 'c':
			if(wins->cpu.visible) {
				wins->cpu.visible = false;
				wins->visiblecount -= 1;
			} else {
				wins->cpu.visible = true;
				wins->visiblecount += 1;
			}
			break;
		case 'C':
			if(wins->cpulong.visible) {
				wins->cpulong.visible = false;
				wins->visiblecount -= 1;
			} else {
				wins->cpulong.visible = true;
				wins->visiblecount += 1;
			}
			break;
		case 'd':
			if(wins->disks.visible) {
				wins->disks.visible = false;
				wins->visiblecount -= 1;
			} else {
				wins->disks.visible = true;
				wins->visiblecount += 1;
			}
			break;
		case 'D':
			if(wins->disklong.visible) {
				wins->disklong.visible = false;
				wins->visiblecount -= 1;
			} else {
				wins->disklong.visible = true;
				wins->visiblecount += 1;
			}
			break;
		case 'f':
			break;
		case 'F':
			break;
		case 'h':
		case 'H':
		case '?':
			if(wins->help.visible) {
				wins->help.visible = false;
				wins->visiblecount -= 1;
			} else {
				wins->help.visible = true;
				wins->visiblecount += 1;
			}
			break;
		case 'i':
			if(wins->sys.visible) {
				wins->sys.visible = false;
				wins->visiblecount -= 1;
			} else {
				wins->sys.visible = true;
				wins->visiblecount += 1;
			}
			break;
		case 'I':
			break;
		case 'k':
			break;
		case 'm':
			if(wins->memory.visible) {
				wins->memory.visible = false;
				wins->visiblecount -= 1;
			} else {
				wins->memory.visible = true;
				wins->visiblecount += 1;
			}
			break;
		case 'M':
			break;
		case 'n':
			if(wins->network.visible) {
				wins->network.visible = false;
				wins->visiblecount -= 1;
			} else {
				wins->network.visible = true;
				wins->visiblecount += 1;
			}
			break;
		case 'N':
			if(wins->netlong.visible) {
				wins->netlong.visible = false;
				wins->visiblecount -= 1;
			} else {
				wins->netlong.visible = true;
				wins->visiblecount += 1;
			}
			break;
		case 'o':
			break;
		case 'q':
			exitapp();
			//break;
		case 'r':
			if(state->topmode == TOP_MODE_C) {
				state->topmode = TOP_MODE_NONE;
				wins->top.visible = false;
				wins->visiblecount -= 1;
			} else {
				if(!wins->top.visible) {
					wins->top.visible = true;
					wins->visiblecount += 1;
				}
				state->topmode = TOP_MODE_C;
			}
			break;
		case 'R':
			if(state->topmode == TOP_MODE_D) {
				state->topmode = TOP_MODE_NONE;
				wins->top.visible = false;
				wins->visiblecount -= 1;
			} else {
				if(!wins->top.visible) {
					wins->top.visible = true;
					wins->visiblecount += 1;
				}
				state->topmode = TOP_MODE_D;
			}
			break;
		case 't':
			if(state->topmode == TOP_MODE_A) {
				state->topmode = TOP_MODE_NONE;
				wins->top.visible = false;
				wins->visiblecount -= 1;
			} else {
				if(!wins->top.visible) {
					wins->top.visible = true;
					wins->visiblecount += 1;
				}
				state->topmode = TOP_MODE_A;
			}
			break;
		case 'T':
			if(state->topmode == TOP_MODE_B) {
				state->topmode = TOP_MODE_NONE;
				wins->top.visible = false;
				wins->visiblecount -= 1;
			} else {
				if(!wins->top.visible) {
					wins->top.visible = true;
					wins->visiblecount += 1;
				}
				state->topmode = TOP_MODE_B;
			}
			break;
		case 'v':
			break;
		case 'w':
			break;
		case 'z':
			break;
		case '0':
			break;
		case '1':
			break;
		case '2':
			break;
		case '3':
			break;
		case '4':
			break;
		case '5':
			break;
		case '+':
			state->refresh = state->refresh * 2;
			state->refreshms = state->refresh * 1000;
			state->pendingchanges = true;
			break;
		case '-':
			if(state->refresh > MINIMUM_REFRESH_RATE) {
				state->refresh = state->refresh / 2;
				state->refreshms = state->refresh * 1000;
				state->pendingchanges = true;
			}
			break;
	}
	if(wins->visiblecount) {
		wins->welcome.visible = false;
	} else {
		wins->welcome.visible = true;
	}
}

static void processenvars(struct uiwins *wins, struct nmondstate *state)
{
	if(getenv("NMONDEBUG") != NULL) {
		state->debug = true;
	}

	char *envar = getenv("NMOND");
	if(envar) {
		for (int i = 0; i < (int)strlen(envar); ++i) {
			setwinstate(wins, state, envar[i]);
		}
	} else {
		envar = getenv("NMON");
		if(envar) {
			for (int i = 0; i < (int)strlen(envar); ++i) {
				switch(envar[i]) {
					case 'l':
						setwinstate(wins, state, 'C');
						break;
					case 'r':
						setwinstate(wins, state, 'i');
						break;
					case 'u':
						setwinstate(wins, state, 'T');
						break;
					default:
						setwinstate(wins, state, envar[i]);
						break;
				}
			}
		}
	}
}

//~~~~~~
// MAIN
//~~~~~~

int main(int argc, char **argv)
{
	// first thing, prepare to be interupted
	setinterupthandlers();

	// initialize the ncurses environment
	initscr();
	// initialize color windows, if available
	if(has_colors()) {
		start_color();
		use_default_colors();
		init_pair((short)0,  COLOR_WHITE,   -1);
		init_pair((short)1,  COLOR_RED,     -1);
		init_pair((short)2,  COLOR_GREEN,   -1);
		init_pair((short)3,  COLOR_YELLOW,  -1);
		init_pair((short)4,  COLOR_BLUE,    -1);
		init_pair((short)5,  COLOR_MAGENTA, -1);
		init_pair((short)6,  COLOR_CYAN,    -1);
		init_pair((short)7,  COLOR_WHITE,   -1); 
		init_pair((short)8,  COLOR_WHITE,   COLOR_RED);
		init_pair((short)9,  COLOR_WHITE,   COLOR_GREEN);
		init_pair((short)10, COLOR_WHITE,   COLOR_BLUE);
		init_pair((short)11, COLOR_BLACK,   COLOR_YELLOW);
		init_pair((short)12, COLOR_BLACK,   COLOR_CYAN);
	}
	// read input character by character
	cbreak();
	// do not echo out read cahracters
	noecho();
	// move the cursor to the top left
	move(0, 0);

	// initialize the app state
	struct nmondstate currentstate = NMONDSTATE_INIT;
	currentstate.color = has_colors();
	currentstate.timenow = time(NULL);
	currentstate.user = getlogin();
	// set wait time for getch()
	timeout(currentstate.refreshms);

	// initialize system information data structures
	struct syshw thishw = SYSHW_INIT;
	getsyshwinfo(&thishw);
	struct syskern thiskern = SYSKERN_INIT;
	getsyskerninfo(&thiskern);
	struct sysres thisres = SYSRES_INIT;
	getsysresinfo(&thisres);
	struct sysnet thisnet = SYSNET_INIT;
	getsysnetinfo(&thisnet);
	size_t processcount = 0;
	struct sysproc **thisproc = NULL;
	struct hashitem *thishash = hashtnew();
	thisproc = getsysprocinfoall(&processcount, thisproc, &thishash, thisres.percentallcpu, &thisres);

	// initialize main() variables
	char hostname[22];
	gethostname(hostname, sizeof(hostname));
	bool pendingdata = false;
	int pressedkey = 0;
	int lastpressedkeya = 0;
	int lastpressedkeyb = 0;
	int cpulongitter = 0;
	int netlongitter = 0;
	int disklongitter = 0;
	int	networks = 0;
	int	flash_on = 0;
	int	show_raw = 0;
	int x = 1;
	int xoffset = 0;

	// TODO: do we want to move theses to setwinstate and create/destroy on show/hide?
	// initialzie window data structures
	struct uiwins wins = UIWINS_INIT;
	wins.welcome.win = newpad(22, MAXCOLS);
	wins.welcome.visible = true;
	wins.help.win = newpad(20, MAXCOLS);
	wins.cpu.win = newpad((thisres.cpucount+3), MAXCOLS);
	wins.cpulong.win = newpad(21, MAXCOLS);
	wins.disks.win = newpad(3, MAXCOLS);
	wins.disklong.win = newpad(21, MAXCOLS);
	// wins.diskgroup.win = newpad(MAXROWS, MAXCOLS);
	// wins.diskmap.win = newpad(24, MAXCOLS);
	// wins.filesys.win = newpad(MAXROWS, MAXCOLS);
	// wins.kernel.win = newpad(5, MAXCOLS);
	wins.memory.win = newpad(3, MAXCOLS);
	// wins.memlarge.win = newpad(20, MAXCOLS);
	// wins.memvirtual.win = newpad(20, MAXCOLS);
	// wins.neterrors.win = newpad(MAXROWS, MAXCOLS);
	// wins.netfilesys.win = newpad(25, MAXCOLS);
	wins.network.win = newpad(MAXROWS, MAXCOLS);
	wins.netlong.win = newpad(21, MAXCOLS);
	wins.top.win = newpad(MAXROWS, MAXCOLS);
	wins.sys.win = newpad(10, MAXCOLS);
	// wins.warn.win = newpad(8, MAXCOLS);

	// change settings based upon environment variables
	processenvars(&wins, &currentstate);
	// repaint on next refresh
	clear();
	// refresh the display
	refresh();

	// Main program loop
	for(;;) {
		// Reset the cursor position to top left
		// x = 1;
		x = 1 + xoffset;

		// update the header
		uiheader(&stdscr, 0, currentstate.color, flash_on, hostname, "", currentstate.refresh, time(0));

		// don't update too much (not every keypress)
		currentstate.timenow = time(NULL);
		currentstate.elapsed = currentstate.timenow - currentstate.timelast;
		if (pressedkey || (currentstate.elapsed > MINIMUM_TIME_ELAPSED) || (currentstate.timelast <= 0)) {
			currentstate.timelast = time(NULL);

			// TODO: only check statistics which are used
			// update system information data structures
			getsyshwinfo(&thishw);
			getsyskerninfo(&thiskern);
			getsysresinfo(&thisres);
			getsysnetinfo(&thisnet);
			processcount = 0;
			thisproc = getsysprocinfoall(&processcount, thisproc, &thishash, thisres.percentallcpu, &thisres);

			// data changes are pending gui update
			pendingdata = true;

			// flash on/off once per itteration
			flash_on = flash_on ? false : true;

			// update the in-use panes
			if(wins.welcome.visible) {
				uiwelcome(&wins.welcome.win, &x, COLS, LINES, currentstate.color, thishw);
			}
			if (wins.help.visible) {
				uihelp(&wins.help.win, &x, COLS, LINES);
			}
			if (wins.sys.visible) {
				uisys(&wins.sys.win, &x, COLS, LINES, thishw, thiskern);
			}
			if (wins.cpulong.visible) {
				uicpulong(&wins.cpulong.win, &x, COLS, LINES, &cpulongitter, currentstate.color, thisres, pendingdata);
			}
			if (wins.disklong.visible) {
				uidisklong(&wins.disklong.win, &x, COLS, LINES, &disklongitter, currentstate.color, \
					(unsigned int)(thisres.diskuser - thisres.diskuserlast), \
					(unsigned int)(thisres.diskusew - thisres.diskusewlast), \
					pendingdata);
			}
			if (wins.netlong.visible) {
				uinetlong(&wins.netlong.win, &x, COLS, LINES, &netlongitter, currentstate.color, thisnet, pendingdata);
			}
			if (wins.cpu.visible) {
				uicpu(&wins.cpu.win, &x, COLS, LINES, currentstate.color, thisres, show_raw);
			}
			if (wins.memory.visible) {
				uimemory(&wins.memory.win, &x, COLS, LINES, currentstate.color, thisres.memused, thishw.memorysize);
			}
			if (wins.disks.visible) {
				uidisks(&wins.disks.win, &x, COLS, LINES, currentstate.color, \
					(unsigned int)(thisres.diskuser - thisres.diskuserlast), \
					(unsigned int)(thisres.diskusew - thisres.diskusewlast));
			}


			if (wins.diskgroup.visible) {
				uidiskgroup(&wins.diskgroup.win, &x, COLS, LINES);
			}
			if (wins.diskmap.visible) {
				uidiskmap(&wins.diskmap.win, &x, COLS, LINES);
			}
			if (wins.filesys.visible) {
				uifilesys(&wins.filesys.win, &x, COLS, LINES);
			}
			if (wins.kernel.visible) {
				uikernel(&wins.kernel.win, &x, COLS, LINES);
			}
			if (wins.memlarge.visible) {
				uimemlarge(&wins.memlarge.win, &x, COLS, LINES);
			}
			if (wins.memvirtual.visible) {
				uimemvirtual(&wins.memvirtual.win, &x, COLS, LINES);
			}
			if (wins.netfilesys.visible) {
				uinetfilesys(&wins.netfilesys.win, &x, COLS, LINES);
			}


			if (wins.network.visible) {
				uinetwork(&wins.disks.win, &x, COLS, LINES, currentstate.color, thisnet);
				/*
				int errors = 0;
				for (int i = 0; i < networks; i++) {
					// errors += p->ifnets[i].if_ierrs - q->ifnets[i].if_ierrs
					// + p->ifnets[i].if_oerrs - q->ifnets[i].if_oerrs
					// + p->ifnets[i].if_ocolls - q->ifnets[i].if_ocolls;
				}
				if(errors) {
					currentstate.neterrors = 3;
				}

				if (currentstate.neterrors) {
					uineterrors(&wins.neterrors.win, &x, COLS, LINES);
				}
				*/
			}
			if (wins.top.visible) {
				// wclear(wins.top.win);
				uitop(&wins.top.win, &x, COLS, LINES, currentstate.color, thisproc, \
					(int)processcount, currentstate.topmode, pendingdata, currentstate.user);
			}


			if (wins.warn.visible) {
				uiwarn(&wins.warn.win, &x, COLS, LINES);
			}

			// all data changes posted by here
			pendingdata = false;
			
			// underline the end of the stats area border
			if(x < LINES-2) {
				mvwhline(stdscr, x, 1, ACS_HLINE, COLS-2);
			}
			wmove(stdscr, 0, 0);
			wrefresh(stdscr);
			doupdate();

			// clear pressed key
			pressedkey = 0;
		}

		// handle input
		pressedkey = getch();
		if(pressedkey) {
			//
			// TODO: catch mouse movement (up/down key presses) and prevent constant screen updates
			//       (do something better than what is currently implemented)
			//
			if((pressedkey != ERR) && (pressedkey != 0x1b) && (lastpressedkeya != 0x1b) && (lastpressedkeyb != 0x1b)) {
				// move the cursor back
				wmove(stdscr, 0, 0);
				// update app state
				setwinstate(&wins, &currentstate, pressedkey);

				// un-underline the end of the stats area border
				if(x < LINES-2) {
					mvwhline(stdscr, x, 1, ' ', COLS-2);
				}

				// if(pressedkey == 'a') {
				// 	++xoffset;
				// } else if(pressedkey == 's') {
				// 	--xoffset;
				// }

				lastpressedkeyb = lastpressedkeya;
				lastpressedkeya = pressedkey;
			} else {
				lastpressedkeyb = lastpressedkeya;
				lastpressedkeya = pressedkey;
				pressedkey = 0;
			}
		}

		// handle app state changes
		if(currentstate.pendingchanges) {
			timeout(currentstate.refreshms);
			currentstate.pendingchanges = false;
		}
	}
}
