/**
* Merge back into nmond.h when it is clean and remove
*/

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include "uicurses.h"
//
// DEINFES AND STRUCTS
//

struct nmondsettings {
	bool pendingchanges;

	int refresh;

	bool debug;
};
#define NMONDSETTINGS_INIT { false, false }

//
// FUNCTIONS
// 

static void interupthundler(int signum)
{
	int child_pid;
	int waitstatus;
	
	// window size change
	// TODO: clean up
	if (signum == SIGWINCH) {
		endwin(); // stop + start curses so it works out the # of row and cols
		initscr();
		cbreak();
		signal(SIGWINCH, interupthundler);
		// COLOUR colour = has_colors();
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
	signal(SIGUSR1, interupthundler);
	signal(SIGUSR2, interupthundler);
	signal(SIGINT, interupthundler);
	signal(SIGWINCH, interupthundler);
	signal(SIGCHLD, interupthundler);
}

static void setwinstate(struct uiwinsets *winsets, struct nmondsettings *settings, char input)
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

static void getenvars(struct uiwinsets *winsets, struct nmondsettings *settings)
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

	char envarstr[16] = "                ";
	int	envarlen = 0;
	if(envar != 0) {
		strcpy(envarstr, envar); // TODO: FIXME, not memory safe
		envarlen = strlen(envarstr);
	} else {
		envarlen = 0;
	}

	for (int i = 0; i < envarlen; ++i) {
		setwinstate(winsets, settings, envarstr[i]);
	}
}

static int getinput(struct uiwinsets *winsets, struct nmondsettings *settings)
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

static void setappstate(struct nmondsettings *settings, char input)
{
	if(settings->pendingchanges) {

	}
}
