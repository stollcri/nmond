/**
 * nmond.h -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
 *
 * 
 * nmond -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
 *  https://github.com/stollcri/nmond
 *  forked from (near complete rewrite):
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
#include <sys/errno.h>
#include <sys/ioctl.h>
#include "sysinfo.h"
#include "uicli.h"
#include "uicurses.h"

static void handleinterupt(int signum)
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
	}

	endwin();
	exit(0);
}

static void setinterupthandlers()
{
	signal(SIGUSR1, handleinterupt);
	signal(SIGUSR2, handleinterupt);
	signal(SIGINT, handleinterupt);
	signal(SIGWINCH, handleinterupt);
	signal(SIGCHLD, handleinterupt);
}

static void setwinstate(struct uiwinsets *winsets, struct nmondstate *settings, char input)
{
	switch (input) {
		case 'a':
			break;
		case 'A':
			break;
		case 'b':
			break;
		case 'c':
			if(winsets->cpu.visible) {
				winsets->cpu.visible = false;
				winsets->visiblecount -= 1;
			} else {
				winsets->cpu.visible = true;
				winsets->visiblecount += 1;
			}
			break;
		case 'C':
			if(winsets->cpulong.visible) {
				winsets->cpulong.visible = false;
				winsets->visiblecount -= 1;
			} else {
				winsets->cpulong.visible = true;
				winsets->visiblecount += 1;
			}
			break;
		case 'd':
			break;
		case 'D':
			break;
		case 'f':
			break;
		case 'F':
			break;
		case 'h':
		case 'H':
		case '?':
			if(winsets->help.visible) {
				winsets->help.visible = false;
				winsets->visiblecount -= 1;
			} else {
				winsets->help.visible = true;
				winsets->visiblecount += 1;
			}
			break;
		case 'i':
			break;
		case 'I':
			break;
		case 'k':
			break;
		case 'm':
			break;
		case 'M':
			break;
		case 'n':
			break;
		case 'N':
			break;
		case 'o':
			break;
		case 'q':
			nocbreak();
			endwin();
			exit(0); // TODO: maybe some sort of shutdown sequence?
			break;
		case 't':
			break;
		case 'T':
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
			settings->refresh = settings->refresh * 2;
			settings->pendingchanges = true;
			break;
		case '-':
			if(settings->refresh > 1) {
				settings->refresh = settings->refresh / 2;
				settings->pendingchanges = true;
			}
			break;
	}
	if(winsets->visiblecount) {
		winsets->welcome.visible = false;
	} else {
		winsets->welcome.visible = true;
	}
}

static void processenvars(struct uiwinsets *winsets, struct nmondstate *settings)
{
	if(getenv("NMONDEBUG") != NULL) {
		settings->debug = true;
	}

	char *envar;
	if(getenv("NMON") != NULL) {
		// need to convert NMON settings into NMOND settings
		// envar = getenv("NMON");
	}
	// NMOND over rides NMON
	if(getenv("NMOND") != NULL) {
		envar = getenv("NMOND");
	}

	char envarstr[16];
	int	envarlen = 0;
	if(envar != 0) {
		// strcpy(envarstr, envar); // TODO: FIXME, not memory safe, and seg faults :-()
		// envarlen = strlen(envarstr);
	} else {
		envarlen = 0;
	}

	for (int i = 0; i < envarlen; ++i) {
		setwinstate(winsets, settings, envarstr[i]);
	}
}

static int processinput(struct uiwinsets *winsets, struct nmondstate *settings)
{
	int result = 0;

	int	bytes;
	char inputstr[64];
	int inputlen = 0;

	ioctl(fileno(stdin), FIONREAD, &bytes);
	
	if(bytes) {
		inputlen = read(fileno(stdin), inputstr, bytes);
		for (int i = 0; i < inputlen; ++i) {
			setwinstate(winsets, settings, inputstr[i]);
		}
		result = 1;
	}

	return result;
}

static void setappstate(struct nmondstate *settings, char input)
{
	if(settings->pendingchanges) {

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
		init_pair((short)0,(short)7,(short)0); /* White */
		init_pair((short)1,(short)1,(short)0); /* Red */
		init_pair((short)2,(short)2,(short)0); /* Green */
		init_pair((short)3,(short)3,(short)0); /* Yellow */
		init_pair((short)4,(short)4,(short)0); /* Blue */
		init_pair((short)5,(short)5,(short)0); /* Magenta */
		init_pair((short)6,(short)6,(short)0); /* Cyan */
		init_pair((short)7,(short)7,(short)0); /* White */
		init_pair((short)8,(short)0,(short)1); /* Red background, red text */
		init_pair((short)9,(short)0,(short)2); /* Green background, green text */
		init_pair((short)10,(short)0,(short)4); /* Blue background, blue text */
		init_pair((short)11,(short)0,(short)3); /* Yellow background, yellow text */
		init_pair((short)12,(short)0,(short)6); /* Cyan background, cyan text */
	}
	// read input character by character
	cbreak();
	// make reading characters (getch) no-blocking
	//nodelay(w, TRUE);
	// move the cursor to the top left
	move(0, 0);

	// initialize system information data structures
	struct nmondstate currentstate = NMONDSTATE_INIT;
	struct uiwinsets winsets = UIWINSETS_INIT;
	struct syshw thishw = getsyshwinfo();
	struct syskern thiskern = getsyskerninfo();
	struct sysres thisres = SYSRES_INIT;
	getsysresinfo(&thisres);
	size_t processcount = 0;
	struct sysproc thisproc = getsysprocinfoall(processcount);

	// initialize main() variables
	int cpulongitter = 0;
	char hostname[256];
	gethostname(hostname, sizeof(hostname));

	// legacy vaiables which should be considered for removal
	int	first_key_pressed = 0;
	int	networks = 0;
	int	loop = 0;
	int	flash_on = 0;
	int	show_raw = 0;
	int	color = has_colors();
	int x;
	int y;

	// initialzie window data structures
	winsets.welcome.win = newpad(24, MAXCOLS);
	winsets.welcome.visible = true;
	winsets.help.win = newpad(24, MAXCOLS);
	winsets.cpu.win = newpad(MAXROWS, MAXCOLS);
	winsets.cpulong.win = newpad(MAXROWS, MAXCOLS);
	winsets.disks.win = newpad(MAXROWS, MAXCOLS);
	winsets.diskgroup.win = newpad(MAXROWS, MAXCOLS);
	winsets.diskmap.win = newpad(24, MAXCOLS);
	winsets.filesys.win = newpad(MAXROWS, MAXCOLS);
	winsets.kernel.win = newpad(5, MAXCOLS);
	winsets.memory.win = newpad(20, MAXCOLS);
	winsets.memlarge.win = newpad(20, MAXCOLS);
	winsets.memvirtual.win = newpad(20, MAXCOLS);
	winsets.neterrors.win = newpad(MAXROWS, MAXCOLS);
	winsets.netfilesys.win = newpad(25, MAXCOLS);
	winsets.network.win = newpad(MAXROWS, MAXCOLS);
	winsets.top.win = newpad(MAXROWS, (MAXCOLS * 2));
	winsets.sys.win = newpad(20, MAXCOLS);
	winsets.warn.win = newpad(8, MAXCOLS);

	// change settings based upon environment variables
	processenvars(&winsets, &currentstate);
	// repaint on next refresh
	clear();printf("asldkfjalskdf\n");
	// refresh the display
	refresh();

	// Main program loop
	for(;;) {
		// Reset the cursor position to top left
		x = 0;
		y = 0;

		// TODO: only check statistics which are used
		thishw = getsyshwinfo();
		thiskern = getsyskerninfo();
		getsysresinfo(&thisres);
		processcount = 0;
		thisproc = getsysprocinfoall(processcount);

		// don't assume actual sleep time matches refresh rate
		currentstate.timelast = currentstate.timenow;
		currentstate.timenow = time(NULL);
		currentstate.elapsed = currentstate.timenow - currentstate.timenow;

		uiheader(&x, color, flash_on, hostname, currentstate.elapsed, time(0));
		
		if(winsets.welcome.visible) {
			uiwelcome(&winsets.welcome.win, &x, COLS, LINES, color, thishw);
		}
		if (winsets.help.visible) {
			uihelp(&winsets.help.win, &x, COLS, LINES);
		}
		if (winsets.cpulong.visible) {
			uicpulong(&winsets.cpulong.win, &x, COLS, LINES, &cpulongitter, color, thisres);
		}
		if (winsets.cpu.visible) {
			uicpu(&winsets.cpu.win, &x, COLS, LINES, color, thisres, show_raw);
		}



		if (winsets.disks.visible) {
			uidisks(&winsets.disks.win, &x, COLS, LINES);
		}
		if (winsets.diskgroup.visible) {
			uidiskgroup(&winsets.diskgroup.win, &x, COLS, LINES);
		}
		if (winsets.diskmap.visible) {
			uidiskmap(&winsets.diskmap.win, &x, COLS, LINES);
		}
		if (winsets.filesys.visible) {
			uifilesys(&winsets.filesys.win, &x, COLS, LINES);
		}
		if (winsets.kernel.visible) {
			uikernel(&winsets.kernel.win, &x, COLS, LINES);
		}
		if (winsets.memory.visible) {
			uimemory(&winsets.memory.win, &x, COLS, LINES);
		}
		if (winsets.memlarge.visible) {
			uimemlarge(&winsets.memlarge.win, &x, COLS, LINES);
		}
		if (winsets.memvirtual.visible) {
			uimemvirtual(&winsets.memvirtual.win, &x, COLS, LINES);
		}
		if (winsets.netfilesys.visible) {
			uinetfilesys(&winsets.netfilesys.win, &x, COLS, LINES);
		}
		if (winsets.network.visible) {
			uinetwork(&winsets.network.win, &x, COLS, LINES);
			
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
				uineterrors(&winsets.neterrors.win, &x, COLS, LINES);
			}
		}
		if (winsets.sys.visible) {
			uisys(&winsets.sys.win, &x, COLS, LINES, thiskern);
		}
		if (winsets.top.visible) {
			uitop(&winsets.top.win, &x, COLS, LINES);
		}
		if (winsets.warn.visible) {
			uiwarn(&winsets.warn.win, &x, COLS, LINES);
		}

		

		
		/* underline the end of the stats area border */
		if(x < LINES-2)mvwhline(stdscr, x, 1, ACS_HLINE,COLS-2);
		
		wmove(stdscr,0, 0);
		wrefresh(stdscr);
		doupdate();
		
		for (int i = 0; i < currentstate.refresh; i++) {
			sleep(1);

			// stollcri - move to top of program loop
			if(processinput(&winsets, &currentstate)) {
				break;
			}
		}

		if(x<LINES-2) mvwhline(stdscr, x, 1, ' ', COLS-2);
		if(first_key_pressed == 0){
			first_key_pressed=1;
			wmove(stdscr,0, 0);
			wclear(stdscr);
			wmove(stdscr,0,0);
			wclrtobot(stdscr);
			wrefresh(stdscr);
			doupdate();
		}
		
		flash_on = flash_on ? false : true;
	}
}
