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
	// PIDs 0 - 999
	if(value < HASH_TABLE_SIZE) {
		return value;
	}

	// PIDs 1,000 - infinity
	while(value >= HASH_TABLE_SIZE) {
		if(value < HASH_TABLE_SIZE_X10) {
			value = value - HASH_TABLE_SIZE;
		// PIDs 10,000 - infinity
		} else {
			if(value < HASH_TABLE_SIZE_X100) {
				value = value - HASH_TABLE_SIZE_X10;
			// PIDs 100,000 - infinity
			} else {
				value = value - HASH_TABLE_SIZE_X100;
			}
		}
	}
	return value;
}

struct hashitem *hashtnew()
{
	return (struct hashitem *)calloc(sizeof(struct hashitem), HASH_TABLE_SIZE);
}

void hashtadd(struct hashitem *hashtable, int key, void *valoc)
{
	struct hashitem *thishashitem = (struct hashitem *)malloc(sizeof(struct hashitem));
	thishashitem->next = NULL;
	thishashitem->keyhash = hash(key);
	thishashitem->key = key;
	thishashitem->valoc = valoc;

	struct hashitem *hitem = &hashtable[thishashitem->keyhash];
	// go to the last linked list item
	while(hitem->next) {
		hitem = hitem->next;
	}
	// this linked list item is already assigned
	if(hitem->valoc) {
		hitem->next = thishashitem;
	// the head of the linked list
	} else {
		hashtable[thishashitem->keyhash] = *thishashitem;
	}
}

void hashtset(struct hashitem *hashtable, int key, void *valoc)
{
	int keyhash = hash(key);

	struct hashitem *hitem = &hashtable[keyhash];
	if(hitem->key != key) {
		while(hitem->next) {
			hitem = hitem->next;
			if(hitem->key == key) {
				hitem->valoc = valoc;
				break;
			}
		}
		// automatically jump to add it when not found
		hashtadd(hashtable, key, valoc);
	} else {
		hitem->valoc = valoc;
	}
}

void *hashtget(struct hashitem *hashtable, int key)
{
	int keyhash = hash(key);

	struct hashitem *hitem = &hashtable[keyhash];
	if(hitem->key != key) {
		while(hitem->next) {
			hitem = hitem->next;
			if(hitem->key == key) {
				break;
			}
		}
		if(hitem->key != key) {
			return NULL;
		}
	}
	return hitem->valoc;
}
