/*
 * Copied from http://man7.org/linux/man-pages/man2/perf_event_open.2.html
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>


static int perf_event_open(
    struct perf_event_attr *hw_event,
    pid_t pid,
    int cpu,
    int group_fd,
    unsigned long flags) {
    return (int)syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}


int main(int argc, char **argv) {
    struct perf_event_attr pe;
    long long count;
    int fd;

    // initialize perf_event_attr struct
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_INSTRUCTIONS;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    // 2nd, 3rd arguments:
    // pid == 0 and cpu == -1
    // This measures the calling process/thread on any CPU.
    //
    // 4th argument:
    // The group_fd argument allows event groups to be created.  An event
    // group has one event which is the group leader.  The leader is created
    // first, with group_fd = -1.  The rest of the group members are created
    // with subsequent perf_event_open() calls with group_fd being set to
    // the file descriptor of the group leader.
    //
    // 5th argument:
    // The flags argument is formed by ORing together zero or more of the
    // following values (see the man page)
        fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(
            stderr,
            "Error opening leader %llx\nCheck perf_event_paranoid (run perf stat)", pe.config);
        exit(EXIT_FAILURE);
    }

    // start counting
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    // the SUP
    printf("Measuring instruction count for this printf\n");

    // stop counting
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    // read counter (PERF_COUNT_HW_INSTRUCTIONS) value
    read(fd, &count, sizeof(long long));

    printf("Used %lld instructions\n", count);

    close(fd);
}