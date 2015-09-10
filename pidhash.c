/**
 * pidhash.c -- Hash table data structure designed for process IDs
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

#include "pidhash.h"
#include <stdlib.h>

static int hash(int value)
{
	if(value < HASH_TABLE_SIZE) {
		return value;
	}

	while(value >= HASH_TABLE_SIZE) {
		value = value - HASH_TABLE_SIZE;
	}
	return value;
}

struct hashitem *hashtnew()
{
	// static struct hashitem *hashtab[HASH_TABLE_SIZE];
	// return hashtab;
	return (struct hashitem *)calloc(sizeof(struct hashitem), HASH_TABLE_SIZE);
}

void hashtadd(struct hashitem *hashtable, int value, void *valptr)
{
	struct hashitem *thishashitem = (struct hashitem *)malloc(sizeof(struct hashitem));
	thishashitem->next = NULL;
	thishashitem->key = hash(value);
	thishashitem->value = value;
	thishashitem->valptr = valptr;

	struct hashitem *hitem = &hashtable[thishashitem->key];
	// go to the last linked list item
	while(hitem->next) {
		hitem = hitem->next;
	}
	// this linked list item is already assigned
	if(hitem->valptr) {
		hitem->next = thishashitem;
	// the head of the linked list
	} else {
		hashtable[thishashitem->key] = *thishashitem;
	}
}

void *hashtget(struct hashitem *hashtable, int value)
{
	int key = hash(value);

	struct hashitem *hitem = &hashtable[key];
	if(hitem->value != value) {
		while(hitem->next) {
			hitem = hitem->next;
			if(hitem->value == value) {
				break;
			}
		}
		if(hitem->value != value) {
			return NULL;
		}
	}

	return hitem->valptr;
}
