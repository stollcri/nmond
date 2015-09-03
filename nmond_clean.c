/**
* Merge back into nmond.h when it is clean and remove
*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "uicurses.h"
//
// DEINFES AND STRUCTS
//

struct nmondsettings {
	bool pendingchange;

	int refresh;

	bool debug;
};
#define NMONDSETTINGS_INIT { false, false }

//
// FUNCTIONS
// 

static void setwinstate(struct uiwinsets *winsets, struct nmondsettings *settings, char input) {
	switch (input) {
		case 'a':
			break;
		case 'A':
			break;
		case 'b':
			break;
		case 'c':
			winsets->cpu.visible = true;
			winsets->visiblecount += 1;
			break;
		case 'C':
			winsets->cpulong.visible = true;
			winsets->visiblecount += 1;
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
			winsets->help.visible = true;
			winsets->visiblecount += 1;
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
			settings->pendingchange = true;
			break;
		case '-':
			if(settings->refresh > 1) {
				settings->refresh = settings->refresh / 2;
				settings->pendingchange = true;
			}
			break;
	}
}

static void readenvars(struct uiwinsets *winsets, struct nmondsettings *settings) {
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

	char envarstr[64];
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
