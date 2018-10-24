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
#include <mrss.h>
#include <unistd.h>
#include <getopt.h>

#include "podcast.h"
#include "util.h"

int info_flag = 0;

int
get_podcast_list(char * url, podcast_head_t * podcast,int maxdays)
{
  mrss_t *feed;
  CURLcode code;
  mrss_error_t ret;
  mrss_item_t *item;
  char *encoding;
  char *p_str, *p_name, * p_url;
  time_t feed_time;
  time_t now = time(NULL);
  mrss_category_t *tags;
  char taglist[MAX_FILENAME_LENGTH + 1];
  char * p;

  p = strpbrk(url, "\r\n");
  if (p != NULL) {
    *p = '\0';
  }

  
  debug("Parse feed %s\n",url);
  ret = mrss_parse_url_with_options_and_error(url,&feed,NULL,&code);
  if (ret != MRSS_OK){
    warn("%s ",mrss_strerror(ret));
    return 1;
  }

  encoding = feed->encoding;
  for (item = feed->item ; item ; item = item->next) {
    if (feed->version == MRSS_VERSION_2_0 ){

        /* if we have a time contol perform the test */
        feed_time = rfc822_to_time_t(item->pubDate);
        if (feed_time == 0 || maxdays == 0 || 
           (((now - feed_time) / SEC_IN_DAY) <= maxdays)) {
        
          /* extract the filename */
          p_name = NULL;

          /* If we have a gui, use it instead of the enclosure_url */
          if (item->guid != NULL && item->guid_isPermaLink)
            p_url = item->guid;
          else
            p_url = item->enclosure_url;

          for (p_str = p_url ; p_str != NULL ; p_str = strstr(p_str,"/")) {
            p_name = ++p_str;
          }

          /* Get the category/tags */
          memset((void*)taglist,0,MAX_FILENAME_LENGTH+1);
          for (tags = item->category; tags; tags = tags->next) {
            if (strncat(taglist,tags->category,MAX_DESC_LENGTH - strlen(tags->category)-1))
              strcat(taglist,";");
          }
          mrss_free(tags);
      
          debug("   - URL %s\n", p_url);
          podcast_add_item( podcast,
                            item->title,
                            p_url,
                            item->enclosure_length,
                            feed_time,
                            taglist,
                            p_name);
        }
    } else {
      warn("The RSS is not in V2 : %s\n",url);
    }
  } 
  mrss_free(item);

  return 1;
}

size_t write_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  return fwrite(ptr, size, nmemb, stream);
}

void 
get_podcast_item(podcast_item_t * item,char * dir)
{
    char filename[MAX_FILEPATH_LENGTH];
    char tmpFilename[MAX_FILEPATH_LENGTH];
    CURL *curl;
    CURLcode ret ;
    FILE * fd;
    struct tm * dt;
    
    debug("downloading file : %s\n",item->url);
    sprintf(filename,"%s/%s",dir,item->filename);
    sprintf(tmpFilename,"%s.part",filename);
    if (access(filename, F_OK | R_OK) == -1 ) {
      fd = fopen(tmpFilename, "w+");
      if (NULL == fd)
        err(errno,"Can't create file : %s\n",tmpFilename);

      curl = curl_easy_init();
      curl_easy_setopt(curl, CURLOPT_VERBOSE,0);
      curl_easy_setopt(curl, CURLOPT_URL ,item->url);
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
      
      // ret = curl_easy_getinfo(curl);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA ,fd);
      ret = curl_easy_perform(curl);
      fclose(fd);
      if (ret)
        warn("Error %d downloading file : %s",ret,item->url);
      else {
        if ( rename(tmpFilename,filename) ) {
          warn("Can't rename file from %s to %s",tmpFilename,filename);
        } else {
          if (info_flag) {
            /* Add extra informations for the file */
            sprintf(tmpFilename,"%s.info",filename);
            fd = fopen(tmpFilename,"w+");
            if ( NULL == fd)
              warn("Can't create file %s\n",filename);
            else {
              dt = localtime(&item->date);
              fprintf(fd,"Filename : %s\nTitle : %s\nTags : %s\nDate : %s",
                   item->filename,
                   item->title,
                   item->taglist,
                   asctime(dt));

              fclose(fd);
            }
          } 
        }
        
      }

      curl_easy_cleanup(curl);
    }
}

static void
usage(void)
{
  printf("usage: podcastdl  -c podcastlist.cfg -o /some/directory -d <number> [-hilv]\n\n");
  printf(" Options :\n");
  printf(" -c podcastlist.cfg : A plain filename containing the list of podcasts\n");
  printf(" -o /some/directory : The target directory where the files will be saved\n");
  printf(" -l : Set the log to be written in syslog, otherwise log are written to stderr\n");
  printf(" -i : Add an additional  .info file with information about the podcast\n");
  printf(" -d <number> : Set the maximum number of days a podcast can have to be download\n");
  printf(" -v : Verbose mode for debug \n");
  printf(" --version : Print the version number \n");
  exit(1);
}

static void
version(void)
{
  printf("podcastdl %s, wrote by Rodrigo OSORIO <ros@bebik.net>\n",
         PODCASTDL_VERSION_STRING);
  exit(0);
}

static const struct option longOpts[] = {
    { "version", no_argument, NULL, 0 },
    { NULL, no_argument, NULL, 0 }
};


int
main (int argc, char * argv[])
{
  podcast_head_t podcast;
  podcast_item_t * item;
  FILE * fd;
  char * conf_file = NULL;
  char * out_dir   = NULL;
  char * days      = NULL;
  char url[MAX_URL_LENGTH + 1];
  int ch      = 0;
  int maxdays = 0;
  int longIndex;

  if (argc == 1)
    usage();
  while ((ch = getopt_long(argc, argv, "c:o:d:lhiv", longOpts,&longIndex)) != -1)
    switch (ch) {  
      case 'h':
        usage();
        break;
      case 'o':
        out_dir = optarg;
        if (access(out_dir, F_OK | W_OK | R_OK ) == -1 )
          err(1, "%s", out_dir);
        break;
      case 'c':
        conf_file = optarg;
        if (access(conf_file, F_OK | R_OK) == -1 )
          err(1, "%s", conf_file);
        break;
      case 'l':
        syslog_flag++;
        break;
      case 'd':
        days = optarg;
        maxdays=atoi(days);
        break;
      case 'i':
        info_flag++;
        break;
      case 'v':
        verbose_flag++;
        break;
      case 0: /* GNU convention */
        if (strcmp("version",longOpts[longIndex].name ) == 0)
          version();
        break;
      case '?':
      default:
        usage();
    }
  argc -= optind;
  argv += optind;

  if (!out_dir || !conf_file )
    usage();

  podcast_set(&podcast);

  fd = fopen(conf_file,"r");
  if (fd)
  { 
    while ( fgets(url,MAX_URL_LENGTH,fd) )
      get_podcast_list(url,&podcast,maxdays);
  }
  
  PODCAST_FOREACH(item,&podcast) {
    get_podcast_item(item,out_dir);
  }

  podcast_free(&podcast);
  return 0;
}

