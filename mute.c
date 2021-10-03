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
#define USAGE (0)
#define ARMED (1)
/* YYYYMMDD plus null terminator = 9 */
#define COPYSIZE (size_t)(9)
/* Let's get boolean */
#define TRUE (0 == 0)
#define FALSE (!(TRUE))

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

int validate_date(char *pdate)
{
  int rv = FALSE;
  int dd, mm, yy;
  int slen = 0;

  slen = strlen(pdate);
  if(9 != slen) {
    return(FALSE);
  }

  // now we know we have 8 characters plus '\n', so sscanf should be safe
  sscanf(pdate, "%4d%2d%2d", &yy, &mm, &dd); //gak

  struct tm input = {
    .tm_mday = dd,
    .tm_mon = mm - 1,
    .tm_year = yy - 1900,
  };

  time_t t = mktime (&input);
  struct tm *output = localtime(&t);

  rv = TRUE;
  rv = rv && (yy == (output->tm_year + 1900));
  rv = rv && (mm == (output->tm_mon + 1));
  rv = rv && (dd == (output->tm_mday));

  return(rv);
}

int getmutedatefromfile(char *destbuf, size_t destbfsz)
{
  /* read out of file system file */
  /* beware rather non-portable specification of file name */
  FILE *fmutefile;
  char *fline = NULL;
  const char *mfile = ".muterc";
  char actualfname[512];
  size_t linelen = 0;
  int rv = EXIT_FAILURE;

  if(destbfsz < COPYSIZE) {
    fprintf(stderr, "Copy buffer size too small: %zu specified, %zu needed\n", destbfsz, COPYSIZE);
    exit(EXIT_FAILURE);
  }

  if(getparameterfilename(NULL, mfile) > 512) {
    fprintf(stderr, "Parameter file size is likely insane!\n");
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

  if(TRUE == validate_date(fline)) {
    strncpy(destbuf, fline, COPYSIZE);
    destbuf[COPYSIZE-1] = '\0'; /* strncpy does not promise to null-terminate */
    rv = EXIT_SUCCESS;
  } else {
    fprintf(stderr, "%s contains invalid date specification ]%s[\n", mfile, fline);
    rv = EXIT_FAILURE;
  }

  if(fline) {
    free(fline);
  }
  return(rv);
}

int buildandruncmd(int argc, char *argv[])
{
  int argptr = 0, curcmdlen = 0, slen = 0, rv = 0;
  char newclistring[MAX_CMD_SIZE];

  newclistring[0] = '\0';

  for (argptr = 1; argptr < argc; argptr++) {
    slen = strlen(argv[argptr]);

    if((curcmdlen + slen) > MAX_CMD_SIZE) {
      fprintf(stderr, "Command line too long!\n");
      exit(EXIT_FAILURE);
    }
    strcat(newclistring, argv[argptr]);
    strncat(newclistring, " ", 1);
    curcmdlen += slen;
  }
  printf("About to exec ]%s[\n", newclistring);
  rv = system(newclistring);
  return(rv);
}

int notmuted(int p_status)
{
  char datenow[20], datemute[20], paramfile[512];
  int rv, cmpv;

  rv = getparameterfilename(paramfile, ".muterc");
  if(EXIT_FAILURE == rv) {
    fprintf(stderr, "Couldn't get parameter file name");
    exit(EXIT_FAILURE);
  }

  if(p_status == USAGE)
    printf("Using parameter file: %s\n", paramfile);

  getdatenow(datenow, sizeof(datenow));
  if(p_status == USAGE)
    printf("Date now:  %s\n", datenow);

  rv = getmutedatefromfile(datemute, sizeof(datemute));
  if((p_status == USAGE) && (EXIT_SUCCESS == rv))
    printf("Mute Date: %s\n", datemute);
  else
    exit(EXIT_FAILURE);
  
  cmpv = strncmp(datenow, datemute, 8);

  return(cmpv >= 0);
}

/* standard processing - do we mute or not? */
int process(int p_argc, char *p_argv[])
{
  int rv = 0, cmpv = -1;
  
  cmpv = notmuted(ARMED);
  if(cmpv)
    rv = buildandruncmd(p_argc, p_argv);
  else  
    rv = EXIT_SUCCESS; // muted - mute date greater than today so do nothing

  return(rv);
}

/* helptext */
int helptext(void)
{
  int rv;

  rv = notmuted(USAGE);
  if(rv)
      printf("Actions currently active\n");
  else
      printf("Actions currently muted\n");

  // printf("%d\n", rv);
  
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
