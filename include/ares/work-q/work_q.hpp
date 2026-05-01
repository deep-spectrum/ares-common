/**
 * @file work_q.hpp
 *
 * @brief
 *
 * @date 4/9/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_COMMON_WORK_Q_HPP
#define ARES_COMMON_WORK_Q_HPP

#include "task.hpp"
#include <ares/data-structures/queue.hpp>
#include <ares/data-structures/sys/slist.h>
#include <ares/synchronization/spinlock.hpp>
#include <chrono>
#include <functional>
#include <thread>

namespace ares {
using namespace std::chrono_literals;

class WorkQ;
struct Work;
struct WorkFlusher;
struct WorkCanceller;
struct WorkDelayable;

/**
 * The signature for a work item handler function.
 */
using work_handler_t = std::function<void(Work *)>;

/**
 * @class Work
 * Class used to submit work.
 */
struct Work {
    /**
     * Constructor.
     * @param handler The work handler.
     */
    explicit Work(work_handler_t handler);
    Work() = delete;
    friend class WorkQ;
    friend struct WorkDelayable;

    /**
     * @brief Busy state flags from the work item.
     *
     * A zero return value indicates the work item appears to be idle.
     *
     * @return A nonzero value if the work item is busy.
     * @return 0 if the work item appears to be idle.
     *
     * @note This is a live snapshot of state, which may change before the
     * result is checked. Use locks where appropriate.
     */
    [[nodiscard]] int work_busy_get() const;

    /**
     * @brief Test whether a work item is currently pending.
     *
     * Wrapper to determine whether a work item is in a non-idle state.
     *
     * @return `true` if Work::work_busy_get() returns a non-zero value.
     *
     * @note This is a live snapshot of state, which may change before the
     * result is checked. Use locks where appropriate.
     */
    [[nodiscard]] bool work_is_pending() const;

    /**
     * @brief Wait for last-submitted instance to complete.
     *
     * Resubmissions may occur while waiting, including chained submissions
     * (from within the handler).
     *
     * @return `true` if call had to wait for completion.
     * @return `false` if work was already idle.
     *
     * @warning Behavior is undefined if this function is invoked on work from a
     * `work` queue running `work`.
     */
    bool work_flush();

    /**
     * @brief Cancel a work item.
     *
     * This attempts to prevent a pending (non-delayable) work item from being
     * processed by removing it from the work queue. If the item is being
     * processed, the work item will continue to be processed, but resubmissions
     * are rejected until cancellation completes.
     *
     * If this returns zero cancellation is complete, otherwise something
     * (probably a work queue thread) is still referencing the item.
     *
     * See also Work::work_cancel_sync().
     *
     * @return the Work::work_busy_get() status indicating the state of the item
     * after all cancellation steps performed by this call are completed.
     */
    int work_cancel();

    /**
     * @brief Cancel a work item and wait for it to complete.
     *
     * Same as Work::work_cancel() but does not return until cancellation is
     * complete. This can be invoked by a thread after Work::work_cancel to
     * synchronize with a previous cancellation.
     *
     * @return `true` if work was pending (call had to wait for cancellation of
     * a running handler to complete, or scheduled or submitted operations were
     * canceled).
     * @return `false` otherwise.
     *
     * @warning Behavior is undefined if this function is invoked on work from a
     * `work` queue running `work`.
     */
    bool work_cancel_sync();

  private:
    // this is so the lock doesn't get destroyed before objects of this type
    std::shared_ptr<SpinLock> _lock;

    // protected with spinlock
    uint32_t flags = 0;
    WorkQ *queue = nullptr;
    work_handler_t handler = nullptr;
    sys_snode_t node = {nullptr};

    [[nodiscard]] int work_busy_get_locked() const;
    bool work_flush_locked(WorkFlusher *flusher);
    int cancel_async_locked();
    bool cancel_sync_locked(WorkCanceller *canceller);
};

#if DELAYABLE_WORK
struct WorkDelayable {
    explicit WorkDelayable(const work_handler_t &handler);
    explicit WorkDelayable(Work &&work);
    WorkDelayable() = delete;

    [[nodiscard]] int work_busy_get() const;
    [[nodiscard]] bool work_is_pending() const;
    // todo: expires_get()
    // todo: remaining_get()
    bool work_flush();
    int work_cancel();
    bool work_cancel_sync();

    friend WorkDelayable *work_delayable_from_work(Work *work);

  private:
    Work work;
    std::chrono::milliseconds timeout{};
    WorkQ *queue = nullptr;

    [[nodiscard]] int work_busy_delayable_get_locked() const;
    int work_cancel_async_locked();
    bool unschedule_locked();
};
#endif // DELAYABLE_WORK

struct WorkQConfig {
    const char *name = "";
    bool no_yield = false;
    bool essential = false;

    // unused right now
    std::chrono::milliseconds work_timeout_ms = 0ms;
};

class WorkQ {
  public:
    WorkQ();
    ~WorkQ();

    int submit(Work *work);
    void start(const WorkQConfig *config);
    void run(const WorkQConfig *config);
    [[nodiscard]] std::thread::id queue_thread_get() const;
    int queue_drain(bool plug);
    int queue_unplug();
    int stop();
    int stop(std::chrono::milliseconds timeout);
#if DELAYABLE_WORK
    int schedule(WorkDelayable *dwork, std::chrono::milliseconds delay);
    int reschedule(WorkDelayable *dwork, std::chrono::milliseconds delay);
#endif // DELAYABLE_WORK

    [[nodiscard]] bool plugged() const;

    friend struct Work;
    friend struct WorkDelayable;

  private:
    Task<void(WorkQ *)> _thread;
    // this is so the lock doesn't get destroyed before objects of this type
    std::shared_ptr<SpinLock> _lock;

    // protected with spinlock
    sys_slist_t pending{};
    bounded_queue<uint8_t, 1, true> notifyq;
    bounded_queue<uint8_t, 1, true> drainq;
    uint32_t flags = 0;

    static int submit_locked(Work *work, WorkQ **queue);
    int submit_locked(Work *work);

    static void init_flusher(WorkFlusher *flusher);
    static void init_canceller(WorkCanceller *canceller, Work *work);
    void flusher_locked(Work *work, WorkFlusher *flusher);
    void notify_locked();
    void remove_locked(Work *work);

    static void work_queue_main(WorkQ *queue);

    static void finalize_flush_locked(Work *work);
    static void finalize_cancel_locked(Work *work);
};

#if SYS_WORK_QUEUE
extern WorkQ sys_work_q;

int work_submit(Work *work);
#if DELAYABLE_WORK
int work_schedule(WorkDelayable *dwork, std::chrono::milliseconds delay);
int work_reschedule(WorkDelayable *dwork, std::chrono::milliseconds delay);
#endif // DELAYABLE_WORK
#endif // SYS_WORK_QUEUE

int work_submit_to_queue(WorkQ *queue, Work *work);
#if DELAYABLE_WORK
int work_schedule_for_queue(WorkQ *queue, WorkDelayable *dwork,
                            std::chrono::milliseconds delay);
int work_reschedule_for_queue(WorkQ *queue, WorkDelayable *dwork,
                              std::chrono::milliseconds delay);
WorkDelayable *work_delayable_from_work(Work *work);
#endif // DELAYABLE_WORK
} // namespace ares

#endif // ARES_COMMON_WORK_Q_HPP
