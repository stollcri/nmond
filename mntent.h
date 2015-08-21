#ifndef _MNTENT_H
#define _MNTENT_H

#include <stdio.h>

struct mntent {
        char *mnt_fsname;
        char *mnt_dir;
        char *mnt_type;
        char *mnt_opts;
        int mnt_freq;
        int mnt_passno;
};

struct mntent *getmntent (FILE *fp);
FILE *setmntent (const char *filename, const char *type);
int endmntent(FILE *fp);
char * hasmntopt (const struct mntent *mnt, const char *option);

//Dummy
#define _PATH_MOUNTED "/etc/mtab"

#endif /* _MNTENT_H */