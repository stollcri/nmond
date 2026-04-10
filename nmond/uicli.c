/**
 * uicli.c -- Command line user interface elements of nmond
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

#include "uicli.h"
#include <stdio.h>

void uiclhint()
{
	printf("\nUsage: nmond [-h] [-V] [-b] [-s <seconds>]\n\n");
	printf("\t-h            show help information\n");
	printf("\t-V            show version\n");
	printf("\t-b            black and white mode\n");
	printf("\t-s <seconds>  time between refreshes [default 2]\n");
	printf("\n");
	printf("\tOnce running, press 'h' for interactive key commands.\n");
	printf("\n");
}

void uiclhelp(char *version)
{
	printf("nmond %s -- Ncurses System Performance Monitor for Darwin\n\n", version);
	printf("Usage: nmond [-h] [-V] [-b] [-s <seconds>]\n\n");
	printf("Options:\n");
	printf("\t-h            show this help and exit\n");
	printf("\t-V            show version and exit\n");
	printf("\t-b            black and white mode [default is color]\n");
	printf("\t-s <seconds>  time between refreshes [default 2]\n");
	printf("\n");
	printf("Interactive Key Commands:\n");
	printf("\tc / C  CPU load / CPU long-term graph\n");
	printf("\td / D  Disk I/O / Disk I/O long-term graph\n");
	printf("\te      Energy statistics\n");
	printf("\tg      GPU metrics\n");
	printf("\th      Help screen\n");
	printf("\ti      System information\n");
	printf("\tm      Memory and Swap\n");
	printf("\tn / N  Network / Network long-term graph\n");
	printf("\tt / T  Top processes by CPU / with command arguments\n");
	printf("\tr / R  Top processes by memory / with command arguments\n");
	printf("\tb      Toggle black & white mode\n");
	printf("\t+ / -  Increase / decrease refresh rate\n");
	printf("\tq      Quit\n");
	printf("\n");
	printf("Environment Variables:\n");
	printf("\tNMOND  Panel selection on startup (e.g., NMOND=\"cdm\")\n");
	printf("\tNMON   Compatibility alias with key translation from original nmon\n");
	printf("\n");
	printf("Report bugs at: https://github.com/stollcri/nmond/issues\n");
}
