/**
 * inotify example
 *
 * @version 1.0.0
 * @author cunsheng<ljj8384 at gmail.com>
 * @copyright (c) 2013 http://cunsheng.me
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>

#define EVENT_SIZE ( sizeof (struct inotify_event) )
#define PRINT_EVENT(event, mask) if (event & mask) printf(", %s", #mask)

int get_queue_len(int fd) /* {{{ */ {
    int ret;
    int queue_len;

    ret = ioctl(fd, FIONREAD, &queue_len);
    if (ret < 0) {
        perror("ioctl error");
        return 0;
    }
    return queue_len;
} /* }}} */

int main(int argc, char **argv) /* {{{ */ {
    char *path;
    int read_len, i, no;
    int fd, wd;
    int queue_len, queue_len_max;
    char *event_buf;
    char name_buf[512];
    struct inotify_event *event;

    if (argc < 2) {
        printf("Usage: %s <file or dir path>\n", argv[0]);
        exit(1);
    }
    path = argv[1];

    fd = inotify_init();
    if (fd < 0) {
        perror("init failure");
        exit(1);
    }

    wd = inotify_add_watch(fd, path, IN_ALL_EVENTS);
    if (wd < 0) {
        perror("add watch failure");
        exit(1);
    }

    printf("fd %d, wd %d\n", fd, wd);

    printf("== read begin");
    queue_len_max = EVENT_SIZE;
    event_buf = calloc(1, queue_len_max);
    printf(", event buf init size: %d", queue_len_max);
    no = 1;
    while (1) {
        queue_len = get_queue_len(fd);
        if (queue_len > queue_len_max) {
            event_buf = realloc(event_buf, queue_len);
            if (!event_buf) {
                perror("realloc error");
                exit(1);
            }
            queue_len_max = queue_len;
            printf(", event buf realloc: %d", queue_len);
        }

        read_len = read(fd, event_buf, queue_len);
        if (read_len < 0) {
            perror("read event failure");
            exit(1);
        }
        printf(", read len: %d\n", read_len);

        i = 0;
        while (i < read_len) {
            event = (struct inotify_event *) &event_buf[i];

            printf("NO %d, WD %d", no++, event->wd);

            PRINT_EVENT(event->mask, IN_ACCESS);
            PRINT_EVENT(event->mask, IN_ATTRIB);
            PRINT_EVENT(event->mask, IN_CLOSE_WRITE);
            PRINT_EVENT(event->mask, IN_CLOSE_NOWRITE);
            PRINT_EVENT(event->mask, IN_CREATE);
            PRINT_EVENT(event->mask, IN_DELETE);
            PRINT_EVENT(event->mask, IN_DELETE_SELF);
            PRINT_EVENT(event->mask, IN_MODIFY);
            PRINT_EVENT(event->mask, IN_MOVE_SELF);
            PRINT_EVENT(event->mask, IN_MOVED_FROM);
            PRINT_EVENT(event->mask, IN_MOVED_TO);
            PRINT_EVENT(event->mask, IN_OPEN);

            if (event->len) {
                printf(" <name: %s>", event->name);
            } else if (event->mask & IN_ISDIR) {
                printf(" <SELF>");
            }

            PRINT_EVENT(event->mask, IN_IGNORED);
            PRINT_EVENT(event->mask, IN_ISDIR);
            PRINT_EVENT(event->mask, IN_Q_OVERFLOW);
            PRINT_EVENT(event->mask, IN_UNMOUNT);

            printf(", cookie: %d", event->cookie);

            i += EVENT_SIZE + event->len;
            printf("\n");
        }

        fflush(stdout);
        printf("\n== read again");
    }

    inotify_rm_watch(fd, wd);
    close(fd);

} /* }}} */
