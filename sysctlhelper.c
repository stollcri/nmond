/**
 * sysctlhelper.c -- Wrappers for sysctl
 *  huge shout-out to: man 3 sysctl
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

#include "sysctlhelper.h"
#include <assert.h>
#include <stdlib.h>
#include <sys/sysctl.h>

/*
 * Get a character string from sysctl (level 2)
 */
char *stringFromSysctl(int mib0, int mib1)
{
	char *result = NULL;

	int mib[2];
	mib[0] = mib0;
	mib[1] = mib1;
	size_t length = 0;
	int complete = 0;
	int error = 0;

	// catch memory errors (ENOMEM),
	// assert to catch program errors
	while(!complete && !error) {
		assert(result == NULL);
		length = 0;

		// get the expected length of the result
		error = sysctl(mib, 2, NULL, &length, NULL, 0);

		// allocate memory for the result
		if(!error) {
			result = malloc(length);
			if (result == NULL) {
				error = 1;
			}
		}

		// get the result
		if(!error) {
			error = sysctl(mib, 2, result, &length, NULL, 0);
			if(!error) {
				complete = 1;
			} else {
				assert(result != NULL);
				free(result);
				result = NULL;
				
				complete = 0;
				error = 0;
			}
		}
	}

	return result;
}

/*
 * Get a character string from sysctlbyname
 */
char *stringFromSysctlByName(char *name)
{
	char *result = NULL;
	size_t length = 0;
	int complete = 0;
	int error = 0;

	// catch memory errors (ENOMEM),
	// assert to catch program errors
	while(!complete && !error) {
		assert(result == NULL);
		length = 0;

		// get the expected length of the result
		error = sysctlbyname(name, NULL, &length, NULL, 0);

		// allocate memory for the result
		if(!error) {
			result = malloc(length);
			if(result == NULL) {
				error = 1;
			}
		}

		// get the result
		if(!error) {
			error = sysctlbyname(name, result, &length, NULL, 0);
			if(!error) {
				complete = 1;
			} else {
				assert(result != NULL);
				free(result);
				result = NULL;

				complete = 0;
				error = 0;
			}
		}
	}

	return result;
}

/*
 * Get an integer from sysctl (level 2)
 */
unsigned int intFromSysctl(int mib0, int mib1)
{
	unsigned int result = 0;

	int mib[2];
	mib[0] = mib0;
	mib[1] = mib1;
	size_t length = sizeof(result);
	sysctl(mib, 2, &result, &length, NULL, 0);

	return result;
}

/*
 * Get an integer from sysctlbyname
 */
unsigned int intFromSysctlByName(char *name)
{
	unsigned int result = 0;

	size_t length = sizeof(result);
	sysctlbyname(name, &result, &length, NULL, 0);

	return result;
}

/*
 * Get a 64bit integer from sysctlbyname
 */
int64_t int64FromSysctlByName(char *name)
{
	int64_t result = 0;

	size_t length = sizeof(result);
	sysctlbyname(name, &result, &length, NULL, 0);

	return result;
}

/*
 * Get a time value from sysctl (level 2)
 */
struct timeval timevalFromSysctl(int mib0, int mib1)
{
	struct timeval result = { 0, 0 };

	int mib[2];
	mib[0] = mib0;
	mib[1] = mib1;
	size_t length = sizeof(result);
	sysctl(mib, 2, &result, &length, NULL, 0);

	return result;
}
