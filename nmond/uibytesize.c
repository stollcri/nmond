/**
 * uibytesize.c -- Convert byte size to human-readable form
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

#include "uibytesize.h"
#include <stdlib.h>
#include <stdio.h>

#define RESULT_LENGTH 10

char *uireadablebytes(unsigned int bytes)
{
	char *result = malloc(RESULT_LENGTH);
	if(result == NULL) {
		// TODO: handle memory allocation failure
	}
	char *resultsufix = NULL;//malloc(2);
	double resultnumber = 0;

	if(bytes > BYTES_IN_KB_LIMIT) {
		if(bytes > BYTES_IN_MB_LIMIT) {
			if(bytes > BYTES_IN_GB_LIMIT) {
				resultnumber = bytes / BYTES_IN_GB;
				resultsufix = "GB";
			} else {
				resultnumber = bytes / BYTES_IN_MB;
				resultsufix = "MB";
			}
		} else {
			resultnumber = bytes / BYTES_IN_KB;
			resultsufix = "KB";
		}
	} else {
		resultnumber = bytes;
		resultsufix = "  ";
	}

	snprintf(result, RESULT_LENGTH, "%3.2f%s", resultnumber, resultsufix);
	return result;
}

char *uireadablebyteslong(unsigned long bytes)
{
	char *result = malloc(RESULT_LENGTH);
	if(result == NULL) {
		// TODO: handle memory allocation failure
	}
	char *resultsufix = NULL;//malloc(2);
	double resultnumber = 0;

	if(bytes > BYTES_IN_KB_LIMIT) {
		if(bytes > BYTES_IN_MB_LIMIT) {
			if(bytes > BYTES_IN_GB_LIMIT) {
				resultnumber = bytes / BYTES_IN_GB;
				resultsufix = "GB";
			} else {
				resultnumber = bytes / BYTES_IN_MB;
				resultsufix = "MB";
			}
		} else {
			resultnumber = bytes / BYTES_IN_KB;
			resultsufix = "KB";
		}
	} else {
		resultnumber = bytes;
		resultsufix = "  ";
	}

	snprintf(result, RESULT_LENGTH, "%3.3f%s", resultnumber, resultsufix);
	return result;
}

char *uireadablebyteslonglong(unsigned long long bytes)
{
	char *result = malloc(RESULT_LENGTH);
	if(result == NULL) {
		// TODO: handle memory allocation failure
	}
	char *resultsufix = NULL;//malloc(2);
	double resultnumber = 0;

	if(bytes > BYTES_IN_KB_LIMIT) {
		if(bytes > BYTES_IN_MB_LIMIT) {
			if(bytes > BYTES_IN_GB_LIMIT) {
				resultnumber = bytes / BYTES_IN_GB;
				resultsufix = "GB";
			} else {
				resultnumber = bytes / BYTES_IN_MB;
				resultsufix = "MB";
			}
		} else {
			resultnumber = bytes / BYTES_IN_KB;
			resultsufix = "KB";
		}
	} else {
		resultnumber = bytes;
		resultsufix = "  ";
	}

	snprintf(result, RESULT_LENGTH, "%3.3f%s", resultnumber, resultsufix);
	return result;
}
