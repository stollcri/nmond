#ifndef NMOND_H
#define NMOND_H

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

#include <stdbool.h>

#define MINIMUM_REFRESH_RATE 1
#define MINIMUM_TIME_ELAPSED 0.5

struct nmondstate {
	int color;
	int refresh;
	int refreshms;
	int neterrors;

	double timenow;
	double timelast;
	double elapsed;

	int topmode;

	bool pendingchanges;
	bool debug;

	char *user;
};
#define NMONDSTATE_INIT { 0, 2, 2000, 0, 0.0, 0.0, 0.0, 0, false, false, NULL }

#endif
