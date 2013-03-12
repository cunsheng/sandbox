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

#define EVENT_SIZE     ( sizeof (struct inotify_event) )

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
    int read_len, i;
    int fd, wd;
    int queue_len, queue_len_max;
    char *event_buf;
    char name_buf[512];
    struct inotify_event *event;

    if (argc < 2) {
        printf("Usage: %s <File or DIR>\n", argv[0]);
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

    queue_len_max = EVENT_SIZE;
    event_buf = calloc(1, queue_len_max);
    printf("-- event buf init size: %d\n\n", queue_len_max);

    printf("== read begin\n");
    while (1) {
        queue_len = get_queue_len(fd);
        if (queue_len > queue_len_max) {
            event_buf = realloc(event_buf, queue_len);
            if (!event_buf) {
                perror("realloc error");
                exit(1);
            }
            queue_len_max = queue_len;
            printf("-- event buf realloc: %d\n", queue_len);
        }

        read_len = read(fd, event_buf, queue_len);
        if (read_len < 0) {
            perror("read event failure");
            exit(1);
        }
        printf("-- read len: %d\n", read_len);

        i = 0;
        while (i < read_len) {
            event = (struct inotify_event *) &event_buf[i];

            printf("WD: %d, ", event->wd);
            switch (event->mask) {
                case IN_ACCESS:
                    printf("IN_ACCESS");
                    break;
                case IN_ATTRIB:
                    printf("IN_ATTRIB");
                    break;
                case IN_CLOSE_WRITE:
                    printf("IN_CLOSE_WRITE");
                    break;
                case IN_CLOSE_NOWRITE:
                    printf("IN_CLOSE_NOWRITE");
                    break;
                case IN_CREATE:
                    printf("IN_CREATE");
                    break;
                case IN_DELETE:
                    printf("IN_DELETE");
                    break;
                case IN_DELETE_SELF:
                    printf("IN_DELETE_SELF");
                    break;
                case IN_MODIFY:
                    printf("IN_MODIFY");
                    break;
                case IN_MOVE_SELF:
                    printf("IN_MOVED_SELF");
                    break;
                case IN_MOVED_FROM:
                    printf("IN_MOVED_FROM");
                    break;
                case IN_MOVED_TO:
                    printf("IN_MOVED_TO");
                    break;
                case IN_OPEN:
                    printf("IN_OPEN");
                    break;
                default:
                    printf("[ UNKNOW ACT ]");
                    break;
            }

            if (event->len) {
                printf(", name: %s", event->name);
            }

            if (event->mask & IN_IGNORED) {
                printf(", IN_IGNORED");
            }
            if (event->mask & IN_ISDIR) {
                printf(", IN_ISDIR");
            }
            if (event->mask & IN_Q_OVERFLOW) {
                printf(", IN_Q_OVERFLOW");
            }
            if (event->mask & IN_UNMOUNT) {
                printf(", IN_UNMOUNT");
            }

            i += EVENT_SIZE + event->len;
            printf("\n");
        }

        printf("== read again\n");
    }

    inotify_rm_watch(fd, wd);
    close(fd);

} /* }}} */
