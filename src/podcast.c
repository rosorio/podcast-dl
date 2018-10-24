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

/* vim:set ts=2 sw=4 sts=4: */

#include "podcast.h"
#include "util.h"


podcast_item_t * 
podcast_add_item( podcast_head_t * podcast, char * title, char * url, int length, 
                  time_t date, char * taglist, char * filename)
{

  podcast_item_t * item;    

  if(strlen(url) > MAX_URL_LENGTH)
  {
    warn("The URL string is too long : %s",url);
    return NULL;
  }

  if(title && strlen(title) > MAX_DESC_LENGTH)
  {
    warn("The title  string is too long : %s",url);
    return NULL;
  }

  item = (podcast_item_t *)malloc(sizeof(podcast_item_t));    
  if(NULL == item)
    err(errno,"Memory allocation error");

  /* Fill the struct */
  item->next = NULL;
  item->date    = date;
  item->length  = length;
  strcpy(item->url,url);
  strcpy(item->title,title?title:"");
  strcpy(item->taglist,taglist);
  strcpy(item->filename,filename);

  /* Add the node to the list */
  if (podcast->last)
    podcast->last->next = item;
  if (NULL == podcast->first)
    podcast->first = item;

  /* This is the last addition */
  podcast->last = item;  
  /* Increase the counter */
  podcast->count++;

  return item;
}

void 
podcast_set( podcast_head_t * podcast )
{
  podcast->first = NULL;
  podcast->last  = NULL;
  podcast->count = 0;
}

void 
podcast_free( podcast_head_t * podcast )
{
  podcast_item_t *temp,
                 *item = podcast->first;

  while(item) {
    temp = item; 
    item = item->next;
    free(temp);
  }
  
  podcast->first = NULL;
  podcast->last  = NULL;
  podcast->count = 0;
}
