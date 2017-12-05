#pragma ident "$Id: dir.c,v 1.2 2015/03/06 23:05:44 dechavez Exp $"
/*======================================================================
 *
 *  Portable directory operations
 *
 *====================================================================*/
#include "util.h"

#ifdef HAVE_DIRENT

LNKLST *utilListDirectory(char *path)
{
DIR *dir;
struct dirent *dp;
struct stat sbuf;
LNKLST *list;

    if (path == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (!utilDirectoryExists(path)) {
        errno = ENOENT;
        return NULL;
    }

    if ((dir = opendir(path)) == NULL) return NULL;
    if ((list = listCreate()) == NULL) return NULL;

    for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
        if (!listAppend(list, dp->d_name, strlen(dp->d_name)+1)) {
            listDestroy(list);
            return NULL;
        }
    }
    closedir(dir);
    
    if (!listSetArrayView(list)) {
        listDestroy(list);
        return NULL;
    }

    return list;
}

BOOL utilRemoveDirectoryTree(char *dirname)
{
DIR *dir;
struct dirent *dp;
struct stat sbuf;
char path[MAXPATHLEN+1];

    if (dirname == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!utilDirectoryExists(dirname)) {
        errno = ENOENT;
        return FALSE;
    }

    if ((dir = opendir(dirname)) == NULL) return FALSE;

    for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
        if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
            snprintf(path, (size_t) MAXPATHLEN, "%s/%s", dirname, dp->d_name);
            if (utilDirectoryExists(dp->d_name)) {
                utilRemoveDirectoryTree(path);
            } else {
                if (unlink(path) != 0) return FALSE;
            }
        }
    }
    closedir(dir);

    if (rmdir(dirname) != 0) return FALSE;
    
    return TRUE;
}

# else

LNKLST *utilListDirectory(char *path)
{

#error "utilListDirectory not implemented for this platform"

}

#endif /* !HAVE_DIRENT */

#ifdef DEBUG_TEST

static void Process(char *path)
{
int i;
char *name;
LNKLST *list;

    if ((list = utilListDirectory(path)) == NULL) {
        printf("utilListDirectory: '%s': %s\n", path, strerror(errno));
        return;
    }

    printf("path='%s'\n", path);
    for (i = 0; i < list->count; i++) {
        name = (char *) list->array[i];
        printf("%s\n", name);
    }

    listDestroy(list);
}

int main(int argc, char **argv)
{
int i;

    if (argc == 1) {
        Process(".");
    } else {
        for (i = 1; i < argc; i++) Process(argv[i]);
    }
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: dir.c,v $
 * Revision 1.2  2015/03/06 23:05:44  dechavez
 * introduced utilRemoveDirectoryTree()
 *
 * Revision 1.1  2006/12/12 22:41:11  dechavez
 * initial release
 *
 */
