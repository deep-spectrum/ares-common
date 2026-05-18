/**
 * @file thread_utils.hpp
 *
 * @brief
 *
 * @date 5/7/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_THREAD_UTILS_H
#define ARES_THREAD_UTILS_H

#include <thread>

enum thread_prio {
    HIGH,
    MED,
    LOW,
};

int change_thread_prio(thread_prio prio);
int pin_cpu(std::thread &thread, int cpu);
int pin_cpu(int cpu);

#endif // ARES_THREAD_UTILS_H
