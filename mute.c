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

int getparameterfile(char *p_buf)
{
        int rv = 0;
        char *homedir = getenv("HOME");

        if(homedir != NULL) { //environment method
                if(!p_buf) {
                        rv = strlen(homedir);
                } else {
                        rv = EXIT_SUCCESS;
                        strcpy(p_buf, homedir);
                }
        } else { //fallback PW method
                uid_t useruid = getuid();
                struct passwd *pw = getpwuid(uid);
                if(pw == NULL) {
                        fprintf(stderr, "Couldn't get home directory from userid\n");
                        exit(EXIT_FAILURE);
                }
                if(!p_buf) {
                        rv = strlen(pw->pw_dir);
                } else {
                        rv = EXIT_SUCCESS;
                        strcpy(p_bug, pw->pw_dir);
                }
        }
        return(rv);
}

/* read out of file system file */
/* beware horribly non-portable specification of file name */
int getmutedatefromfile(char *destbuf, size_t destbfsz)
{
        FILE *fmutefile;
        char *fline = NULL;
        const char mfile[] = "/Users/mauvedeity/.muterc";
        size_t linelen = 0;

        fmutefile = fopen(mfile, "r");
        if (NULL == fmutefile) {
                fprintf(stderr, "Couldn't open %s for reading\n", mfile);
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

/* main */
int main(int argc, char *argv[])
{
        char datenow[20], datemute[20], homedir[512];
        printf("mute.c\n");
        getdatenow(datenow, sizeof(datenow));
        printf("Date now: ]%s[\n", datenow);
        getmutedatefromfile(datemute, sizeof(datemute));
        printf("Mute Date: ]%s[\n", datemute);
        printf("Homedir buffer size: %d\n", getparameterfile(NULL));
        (void)getparameterfile(homedir);
        printf("Homedir value: %s\n", homedir);
        return(0);
}
