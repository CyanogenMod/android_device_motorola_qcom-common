#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/stat.h>
#include <private/android_filesystem_config.h>

#define DUMP_PATH "/sys/class/kgsl/kgsl-3d0/snapshot"
#define POLL_FILE DUMP_PATH"/timestamp"
#define DUMP_FILE DUMP_PATH"/dump"
#define COPYSIZE 256

#define MAX_DUMPFILES	10
#define typecheck(x,y) {    \
    typeof(x) __dummy1;     \
    typeof(y) __dummy2;     \
    (void)(&__dummy1 == &__dummy2); }

#define DUMPFILE_DIR	"/data/graphicsdumps"

static int find_and_open_dumpfile()
{
    unsigned long mtime = ULONG_MAX;
    struct stat sb;
    char path[128];
    int fd, i, oldest = 0;

    /*
     * XXX: Our stat.st_mtime isn't time_t. If it changes, as it probably ought
     * to, our logic breaks. This check will generate a warning if that happens.
     */
    typecheck(mtime, sb.st_mtime);

    /*
     * In a single wolf-like pass, find an available slot and, in case none
     * exist, find and record the least-recently-modified file.
     */
    for (i = 0; i < MAX_DUMPFILES; i++) {
        snprintf(path, sizeof(path), DUMPFILE_DIR"/snapshot%02d", i);

        if (!stat(path, &sb)) {
            if (sb.st_mtime < mtime) {
                oldest = i;
                mtime = sb.st_mtime;
            }
            continue;
        }
        if (errno != ENOENT)
            continue;

        fd = open(path, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd < 0)
            continue;	/* raced ? */

        printf("graphicsd - dumping to file[%s]...", path);
        return fd;
    }

    /* we didn't find an available file, so we clobber the oldest one */
    snprintf(path, sizeof(path), DUMPFILE_DIR"/snapshot%02d", oldest);
    fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    printf("graphicsd - dumping to file[%s]...", path);
    return fd;
}

short capture_dump() {
    char buffer[COPYSIZE] = {0};
    int rc = 0;
    int dumpfd, ifd = -1;
    short ret = -1;
    int totalc = 0;

    mkdir(DUMPFILE_DIR, 02755);
    chmod(DUMPFILE_DIR, 02775);
    chown(DUMPFILE_DIR, AID_SYSTEM, AID_SYSTEM);

    dumpfd = find_and_open_dumpfile();

    if(dumpfd >= 0) {
        ifd = open(DUMP_FILE, O_RDONLY);
        if(ifd >= 0) {
            // copy from dump file to destination file
            while((rc = read(ifd, buffer, COPYSIZE)) != 0) {
                totalc += rc;
                write(dumpfd, buffer, rc);
            }
            printf("Success [size: %d]!\n", totalc);
            ret = 0;
            close(ifd);
        } else {
            printf("ERROR! unable to open source file[%s][errno:%d(%s)]\n", DUMP_FILE, errno, strerror(errno));
        }
        close(dumpfd);
        // to commit buffer cache to disk
        sync();
    } else {
        printf("ERROR! unable to open destination file[errno:%d(%s)]\n", errno, strerror(errno));
    }

    return ret;
}

int main(int argc, char* argv[])
{
    int fd = -1;
    struct pollfd fds;
    char buffer[16] = {0};
    int rc;
    short run = 1;

    printf("graphicsd - version: 1.0\n");

    fd = open(POLL_FILE, O_RDONLY);
    if(fd >= 0) {
        // perform initial read to clear poll flag
        rc = read(fd, buffer, sizeof(buffer));
        lseek(fd, 0, SEEK_SET);
        if(rc < 0) {
            printf("graphicsd - ERROR! unable to read[%s][errno: %d(%s)]\n", POLL_FILE, errno, strerror(errno));
            run = 0;
        }
    } else {
        printf("graphicsd - ERROR! unable to open[%s][errno: %d(%s)]\n", POLL_FILE, errno, strerror(errno));
        run = 0;
    }

    while(run) {
        fds.fd = fd;
        fds.events = POLLERR|POLLPRI;
        fds.revents = 0;

        printf("graphicsd - watching [%s][fd: %d]\n", POLL_FILE, fd);
        int poll_val = poll(&fds, 1, -1);
        printf("graphicsd - timestamp file triggered [poll code: %d]\n", poll_val);

        if (poll_val == -1) {
             printf("graphicsd - poll failure, errno = %d (%s)\n", errno, strerror(errno));
        } else if (poll_val > 0) {
            rc = read(fd, buffer, sizeof(buffer));
            buffer[rc-1] = 0; // clear the carriage return
            printf("graphicsd - graphics hw lockup @ timestamp [%s]\n", buffer);
            lseek(fd, 0, SEEK_SET);
            memset(buffer, 0, sizeof(buffer));
            capture_dump();
        }
    }

    close(fd);

    return 0;
}
