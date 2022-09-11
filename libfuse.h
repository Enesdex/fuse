#define FUSE_USE_VERSION 34
#define FUSE_PATH "/tmp/foo"

#include "fuse.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>

int fuse_pipe[2];

int fuse_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi){

    char s;
    read(fuse_pipe[0],&s,1);
    return size;
}

int fuse_getattr(const char *path, struct stat *stbuf,struct fuse_file_info *fi)
{
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0)
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if (strcmp(path + 1, "a") == 0 || strcmp(path + 1, "b") == 0)
    {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = 0x1000;
    }
    else
    {
        res = -ENOENT;
    }

    return res;
}


int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi,
             enum fuse_readdir_flags flags)
{
    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    filler(buf, "a", NULL, 0, 0);
    filler(buf, "b", NULL, 0, 0);

    return 0;
}

static const struct fuse_operations fuse_ops = {
    .getattr        = fuse_getattr,
    .readdir        = fuse_readdir,
    .read           = fuse_read,
};

char *fargs_fuse[] = {NULL, FUSE_PATH, NULL};


void fuse(char **argv){
    rmdir(FUSE_PATH);
    mkdir(FUSE_PATH,0777);

    pipe(fuse_pipe);
    fargs_fuse[0] = argv[0];

    if (!fork()){
        fuse_main(2,fargs_fuse,&fuse_ops,NULL);
    }
}
