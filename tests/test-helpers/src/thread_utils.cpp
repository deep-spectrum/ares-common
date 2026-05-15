/**
 * @file thread_utils.cpp
 *
 * @brief
 *
 * @date 5/7/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <thread_utils.hpp>

extern "C" {
#include <errno.h>
#include <pthread.h>
#include <stdio.h>

static int pin_cpu_pthread(pthread_t tid, int cpu) {
    cpu_set_t cpu_set;
    int ret;

    CPU_ZERO(&cpu_set);
    CPU_SET(cpu, &cpu_set);

    ret = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpu_set);
    if (ret != 0) {
        perror("pthread_setaffinity_np()");
        return -errno;
    }

    return ret;
}

static int set_thread_prio(int new_prio) {
    pthread_t tid = pthread_self();
    struct sched_param param {};
    int policy;
    int prio = 10 + new_prio;

    int ret = pthread_getschedparam(tid, &policy, &param);
    if (ret != 0) {
        perror("pthread_getschedparam()");
        return -errno;
    }

    printf("    policy=%s, priority=%d\n",
           (policy == SCHED_FIFO)    ? "SCHED_FIFO"
           : (policy == SCHED_RR)    ? "SCHED_RR"
           : (policy == SCHED_OTHER) ? "SCHED_OTHER"
                                     : "???",
           param.sched_priority);

    param.sched_priority = prio;
    ret = pthread_setschedparam(tid, SCHED_FIFO, &param);
    if (ret != 0) {
        perror("pthread_setschedparam()");
        return -errno;
    }

    ret = pin_cpu_pthread(tid, 0);
    if (ret < 0) {
        return ret;
    }

    ret = pthread_getschedparam(tid, &policy, &param);
    if (ret != 0) {
        perror("pthread_getschedparam()");
        return -errno;
    }

    printf("    policy=%s, priority=%d\n",
           (policy == SCHED_FIFO)    ? "SCHED_FIFO"
           : (policy == SCHED_RR)    ? "SCHED_RR"
           : (policy == SCHED_OTHER) ? "SCHED_OTHER"
                                     : "???",
           param.sched_priority);

    return 0;
}
}

int change_thread_prio(thread_prio prio) {
    int thread_prio;

    switch (prio) {
    case HIGH:
        thread_prio = 40;
        break;
    case MED:
        thread_prio = 20;
        break;
    case LOW:
        thread_prio = 0;
        break;
    default:
        return -EINVAL;
    }

    return set_thread_prio(thread_prio);
}

int pin_cpu(std::thread &thread, int cpu) {
    return pin_cpu_pthread(thread.native_handle(), cpu);
}
