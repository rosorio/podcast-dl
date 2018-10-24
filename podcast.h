/*
 * Copyright (c) 2010, Rodrigo OSORIO 
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef _PODCAST_H_
#define _PODCAST_H_

#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This value is empiric, there is no limit
 * for URL in the HTTP protocol. See :
 * http://www.boutell.com/newfaq/misc/urllength.html
 * http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.2.2
 */
#define MAX_URL_LENGTH      1024
#define MAX_FILENAME_LENGTH  255
#define MAX_FILEPATH_LENGTH 1024
#define MAX_DESC_LENGTH      255
#define SEC_IN_DAY         86400

typedef struct podcast_item_t podcast_item_t;
typedef struct podcast_head_t podcast_head_t;

/* Data struct for the podcast files */
struct podcast_item_t {
  char title[MAX_DESC_LENGTH + 1];
  char url[MAX_URL_LENGTH + 1];
  char filename[MAX_FILENAME_LENGTH + 1];
  char taglist[MAX_DESC_LENGTH + 1];
  time_t date;
  int    length;
  podcast_item_t *next;
};

struct podcast_head_t {
  podcast_item_t * first;
  podcast_item_t * last;
  int count;
};

#define PODCAST_FOREACH(var, podcast)     \
  for ((var) = (podcast)->first;            \
       (var);                             \
       (var) = (var)->next)

podcast_item_t * podcast_add_item( podcast_head_t * podcast, char * title, char * url, int length, time_t date, char * taglist, char * filename);
void podcast_set( podcast_head_t * podcast );
void podcast_free( podcast_head_t * podcast );

#endif
