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
#include "nmond_clean.c"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <pwd.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fstab.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <net/if.h>

#include "mntent.h"

#include "sysinfo.h"
#include "uicli.h"
#include "uicurses.h"


#ifdef MALLOC_DEBUG
#define MALLOC(argument) mymalloc(argument,__LINE__)
#define FREE(argument) myfree(argument,__LINE__)
#define REALLOC(argument1,argument2) myrealloc(argument1,argument2,__LINE__)
void *mymalloc(int size, int line)
{
	void * ptr;
	ptr= malloc(size);
	fprintf(stderr,"0x%x = malloc(%d) at line=%d\n",ptr,size,line);
	return ptr;
}
void myfree(void *ptr,int line)
{
	fprintf(stderr,"free(0x%x) at line=%d\n",ptr,line);
	free(ptr);
}
void *myrealloc(void *oldptr, int size, int line)
{
	void * ptr;
	ptr= realloc(oldptr,size);
	fprintf(stderr,"0x%x = realloc(0x%x, %d) at line=%d\n",ptr,oldptr,size,line);
	return ptr;
}
#else
#define MALLOC(argument) malloc(argument)
#define FREE(argument) free(argument)
#define REALLOC(argument1,argument2) realloc(argument1,argument2)
#endif /* MALLOC_DEBUG */



int	networks = 0;  	/* number of networks in system  */

char	hostname[256];

int	loop;


int	flash_on     = 0;

int	show_raw    = 0;

#define COLOUR if(colour) // Only use this for single line colour curses calls */
int	colour = 1;	// 1 = using colour curses and

double	doubletime(void)
{
	
	// gettimeofday(&p->tv, 0);
	// return((double)p->tv.tv_sec + p->tv.tv_usec * 1.0e-6);
	return 0.0;
}




void init_pairs()
{
	COLOUR init_pair((short)0,(short)7,(short)0); /* White */
	COLOUR init_pair((short)1,(short)1,(short)0); /* Red */
	COLOUR init_pair((short)2,(short)2,(short)0); /* Green */
	COLOUR init_pair((short)3,(short)3,(short)0); /* Yellow */
	COLOUR init_pair((short)4,(short)4,(short)0); /* Blue */
	COLOUR init_pair((short)5,(short)5,(short)0); /* Magenta */
	COLOUR init_pair((short)6,(short)6,(short)0); /* Cyan */
	COLOUR init_pair((short)7,(short)7,(short)0); /* White */
	COLOUR init_pair((short)8,(short)0,(short)1); /* Red background, red text */
	COLOUR init_pair((short)9,(short)0,(short)2); /* Green background, green text */
	COLOUR init_pair((short)10,(short)0,(short)4); /* Blue background, blue text */
	COLOUR init_pair((short)11,(short)0,(short)3); /* Yellow background, yellow text */
	COLOUR init_pair((short)12,(short)0,(short)6); /* Cyan background, cyan text */
}


/* only place the q=previous and p=currect pointers are modified */
void switcher(void)
{

	if(flash_on)
		flash_on = 0;
	else
		flash_on = 1;

}





	
	int main(int argc, char **argv)
	{
		//
		// DEBUG -- stollcri
		// 

		setinterupthandlers();

		struct nmondstate currentstate = NMONDSETTINGS_INIT;
		struct uiwinsets winsets = UIWINSETS_INIT;
		winsets.welcome.visible = true;
		getenvars(&winsets, &currentstate);



		struct syshw thishw = getsyshwinfo();
		struct syskern thiskern = getsyskerninfo();
		struct sysres thisres = SYSRES_INIT;
		getsysresinfo(&thisres);
		int cpulongitter = 0;
		size_t processcount = 0;
		struct sysproc thisproc = getsysprocinfoall(processcount);


		int	first_key_pressed = 0;

		gethostname(hostname, sizeof(hostname));


				

		initscr();
		// stollcri -- 2015-09-04
		winsets.welcome.win = newpad(24, MAXCOLS);
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


		cbreak();
		move(0, 0);
		refresh();
		COLOUR colour = has_colors();
		COLOUR start_color();
		COLOUR init_pairs();
		clear();
		


		/* To get the pointers setup */
		/* Was already done earlier, DONT'T switch back here to the old pointer! - switcher(); */
		/*checkinput();*/
		clear();
		fflush(NULL);
		int x;
		int y;


		// Main program loop
		for(loop=1; ; loop++) {
			// Reset the cursor position to top left
			x = 0;
			y = 0;



			// stollcri, 2015
			// TODO: only check statistics which are used
			thishw = getsyshwinfo();
			thiskern = getsyskerninfo();
			getsysresinfo(&thisres);
			processcount = 0;
			thisproc = getsysprocinfoall(processcount);

			// don't assume actual sleep time matches refresh rate
			currentstate.timelast = currentstate.timenow;
			currentstate.timenow = doubletime();
			currentstate.elapsed = currentstate.timenow - currentstate.timenow;

			uiheader(&x, colour, flash_on, hostname, currentstate.elapsed, time(0));
			
			if(winsets.welcome.visible) {
				uiwelcome(&winsets.welcome.win, &x, COLS, LINES, colour, thishw);
			}
			if (winsets.help.visible) {
				uihelp(&winsets.help.win, &x, COLS, LINES);
			}
			if (winsets.cpulong.visible) {
				uicpulong(&winsets.cpulong.win, &x, COLS, LINES, &cpulongitter, colour, thisres);
			}
			if (winsets.cpu.visible) {
				uicpu(&winsets.cpu.win, &x, COLS, LINES, colour, thisres, show_raw);
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
				if(getinput(&winsets, &currentstate)) {
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
			
			switcher();
		}
	}
