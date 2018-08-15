/*
                 _             
 _ __ ___  _   _| |_ ___   ___ 
| '_ ` _ \| | | | __/ _ \ / __|
| | | | | | |_| | ||  __/| (__ 
|_| |_| |_|\__,_|\__\___(_)___|
                               
*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

/* defines */
/* this seems to be about right on OS X, Pi reckons bigger */
#define MAX_CMD_SIZE 262144

/* helper functions */
int getdatenow(char *destbuf, size_t destbufsz)
{
  time_t t_now;
  struct tm *locnow;
  int rv;

  t_now = time(NULL);

  if(t_now == (time_t)-1) {
    fprintf(stderr, "Can't get current time\n");
    exit(EXIT_FAILURE);
  }

  locnow = localtime(&t_now);
  if(locnow == NULL) {
    fprintf(stderr, "Local time conversion failed\n");
    exit(EXIT_FAILURE);
  }

  rv = strftime(destbuf, destbufsz, "%Y%m%d", locnow);
  if(!rv) {
    fprintf(stderr, "Local time formatting failed\n");
    exit(EXIT_FAILURE);
  } else {
    return(0);
  }
}

int getparameterfilename(char *p_buf, const char *fname)
{
  int rv = 0;
  char *homedir = getenv("HOME");
  int fnamlen = strlen(fname) + 1; // allow for extra path separator

  if(homedir != NULL) { //environment method
    if(!p_buf) {
      rv = strlen(homedir) + fnamlen;
    } else {
      rv = EXIT_SUCCESS;
      strcpy(p_buf, homedir);
      strcat(p_buf, "/");
      strcat(p_buf, fname);
    }
  } else { //fallback PW method
    uid_t useruid = getuid();
    struct passwd *pw = getpwuid(useruid);
    if(pw == NULL) {
      fprintf(stderr, "Couldn't get home directory from userid\n");
      exit(EXIT_FAILURE);
    }
    if(!p_buf) {
	    rv = strlen((pw->pw_dir) + fnamlen);
    } else {
      rv = EXIT_SUCCESS;
      strcpy(p_buf, pw->pw_dir);
      strcat(p_buf, "/");
      strcat(p_buf, fname);
    }
  }
  return(rv);
}

/* read out of file system file */
/* beware rather non-portable specification of file name */
int getmutedatefromfile(char *destbuf, size_t destbfsz)
{
  FILE *fmutefile;
  char *fline = NULL;
  const char *mfile = ".muterc", actualfname[512];
  size_t linelen = 0;
  int rv = EXIT_FAILURE;

  if(getparameterfilename(NULL, mfile) > 512) {
    fprintf(stderr, "Parameter file size is likely insane!");
    exit(EXIT_FAILURE);
  }

  rv = getparameterfilename((char *)actualfname, mfile);
  if(rv == EXIT_FAILURE) {
    fprintf(stderr, "Failed getting param file name for %s\n", mfile);
      exit(EXIT_FAILURE);
    }

    fmutefile = fopen(actualfname, "r");
    if (NULL == fmutefile) {
      fprintf(stderr, "Couldn't open %s for reading\n", actualfname);
      exit(EXIT_FAILURE);
    }
    linelen = getline(&fline, &linelen, fmutefile);
    fclose(fmutefile);
    strncpy(destbuf, fline, destbfsz);
    if(fline) {
      free(fline);
    }
    return(0);
}

int buildandruncmd(int argc, char *argv[])
{
  int argptr = 0, curcmdlen = 0, slen = 0, rv = 0;
  char newclistring[MAX_CMD_SIZE];

  // build command line - need some way to make the length not
  // overflow and cause me a core dump.

  newclistring[0] = '\0';

  for (argptr = 1; argptr < argc; argptr++) {
    slen = strlen(argv[argptr]);

    if((curcmdlen + slen) > MAX_CMD_SIZE) {
      fprintf(stderr, "Command line too long!\n");
      exit(1);
    }
    strcat(newclistring, argv[argptr]);
    strncat(newclistring, " ", 1);
    curcmdlen += slen;
  }
  printf("About to exec ]%s[\n", newclistring);
  rv = system(newclistring);
  return(rv);
}

/* standard processing - do we mute or not? */
int process(int p_argc, char *p_argv[])
{
  char datenow[20], datemute[20];
  int rv = 0, cmpv = 0;

  getdatenow(datenow, sizeof(datenow));
  // printf("Date now: ]%s[\n", datenow);
  getmutedatefromfile(datemute, sizeof(datemute));
  // printf("Mute Date: ]%s[\n", datemute);

  cmpv = strncmp(datenow, datemute, 8);

  switch(cmpv) {
    case -1:
      rv = EXIT_SUCCESS; // muted - mute date greater than today so do nothing
      break;
    case 0: 
    case 1:         // not muted - mute date less than or equal to today
      rv = buildandruncmd(p_argc, p_argv);
      break;
  }
  return(rv);
}

/* helptext */
int helptext(void)
{
  char datenow[20], datemute[20], paramfile[512];
  int rv, cmpv;

  getdatenow(datenow, sizeof(datenow));
  printf("Date now: ]%s[\n", datenow);

  rv = getparameterfilename(paramfile, ".muterc");

  printf("Using paramter file: %s\n", paramfile);

  getmutedatefromfile(datemute, sizeof(datemute));
  printf("Mute Date: ]%s[\n", datemute);
  cmpv = strncmp(datenow, datemute, 8);

  switch(cmpv) {
    case -1:
      printf("Actions currently muted\n");
      break;
    case 0: 
    case 1: 
      printf("Actions currently active\n");
      break;
  }
  
  return(EXIT_SUCCESS);
}

/* main */
int main(int argc, char *argv[])
{
  /* so, what is argc? */
  int rv;
  
  if(argc == 1) {
    rv = helptext();
  } else {
    rv = process(argc, argv);
  }

  return(rv);
}
