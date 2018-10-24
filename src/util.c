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

#include "util.h"

int syslog_flag = 0; /* 0 == log to stderr */
int verbose_flag = 0; /* 0 == disabled */

void
debug(const char* message, ...)
{
  va_list args;
  va_start(args, message);
  if (verbose_flag) {
    fprintf(stderr,"Debug : ");
    vfprintf(stderr, message, args);
  }
  va_end(args);
}


void
err(int eval, const char* message, ...)
{
  va_list args;
  va_start(args, message);
  if (syslog_flag) {
    vsyslog(LOG_ERR, message, args);
    exit(eval);
    /* NOTREACHED */
  } else {
    verr(eval, message, args);
    /* NOTREACHED */
  }
}

void
warn(const char* message, ...)
{
  va_list args;
  va_start(args, message);
  if (syslog_flag)
    vsyslog(LOG_WARNING, message, args);
  else
    vwarn(message, args);
  va_end(args);
}

/* convert the iso format as the RFC3339 is a subset of it */
time_t
iso8601_to_time_t(char *s)
{
  struct tm date;
  time_t t;
  errno = 0;
  char *pos = strptime(s, "%Y-%m-%dT%H:%M:%S.%fZ", &date);
  if (pos == NULL) {
    /* Modify the last HH:MM to HHMM if necessary */
    if (s[strlen(s) - 3] == ':' ) {
      s[strlen(s) - 3] = s[strlen(s) - 2];
      s[strlen(s) - 2] = s[strlen(s) - 1];
      s[strlen(s) - 1] = '\0';
    }
    pos =strptime(s, "%Y-%m-%dT%H:%M:%S%z", &date);
  }
  if (pos == NULL) {
    errno = EINVAL;
    warn("Convert  ISO8601 '%s' to struct tm failed", s);
    return 0;
  }
  t = mktime(&date);
  if (t == (time_t)-1) {
    errno = EINVAL;
    warn("Convert struct tm (from '%s') to time_t failed", s);
    return 0;
  }
  return t;
}

/* convert RFC822 to epoch time */
time_t
rfc822_to_time_t(char *s)
{
  struct tm date;
  time_t t;
  errno = 0;

  if (s == NULL) {
    warn("Invalide empty date");
    return 0;
  }

  char *pos = strptime(s, "%a, %d %b %Y %T", &date);
  if (pos == NULL) {
    errno = EINVAL;
    warn("Convert RFC822 '%s' to struct tm failed", s);
    return 0;
  }
  t = mktime(&date);
  if (t == (time_t)-1) {
    errno = EINVAL;
    warn("Convert struct tm (from '%s') to time_t failed", s);
    return 0;
  }
  return t;
}
