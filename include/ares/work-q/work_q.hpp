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
#include <ares/util.h>
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

enum WorkStatus : uint32_t {

    /* Bits that represent the work item states.  At least nine of the
     * combinations are distinct valid stable states.
     */
    WORK_RUNNING_BIT = 0,
    WORK_CANCELING_BIT = 1,
    WORK_QUEUED_BIT = 2,
    WORK_DELAYED_BIT = 3,
    WORK_FLUSHING_BIT = 4,

    WORK_MASK = BIT(WORK_DELAYED_BIT) | BIT(WORK_QUEUED_BIT) |
                BIT(WORK_RUNNING_BIT) | BIT(WORK_CANCELING_BIT) |
                BIT(WORK_FLUSHING_BIT),

    /* Static work flags */
    WORK_DELAYABLE_BIT = 8,
    WORK_DELAYABLE = BIT(WORK_DELAYED_BIT),

    /* Dynamic work queue flags */
    WORK_QUEUE_STARTED_BIT = 0,
    WORK_QUEUE_STARTED = BIT(WORK_QUEUE_STARTED_BIT),
    WORK_QUEUE_BUSY_BIT = 1,
    WORK_QUEUE_BUSY = BIT(WORK_QUEUE_BUSY_BIT),
    WORK_QUEUE_DRAIN_BIT = 2,
    WORK_QUEUE_DRAIN = BIT(WORK_QUEUE_DRAIN_BIT),
    WORK_QUEUE_PLUGGED_BIT = 3,
    WORK_QUEUE_PLUGGED = BIT(WORK_QUEUE_PLUGGED_BIT),
    WORK_QUEUE_STOP_BIT = 4,
    WORK_QUEUE_STOP = BIT(WORK_QUEUE_STOP_BIT),

    /* Static work queue flags */
    WORK_QUEUE_NO_YIELD_BIT = 8,
    WORK_QUEUE_NO_YIELD = BIT(WORK_QUEUE_NO_YIELD_BIT),

    /* Transient work flags */
    WORK_RUNNING = BIT(WORK_RUNNING_BIT),
    WORK_CANCELING = BIT(WORK_CANCELING_BIT),
    WORK_QUEUED = BIT(WORK_QUEUED_BIT),
    WORK_DELAYED = BIT(WORK_DELAYED_BIT),
    WORK_FLUSHING = BIT(WORK_FLUSHING_BIT),
};

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
    friend int work_submit_to_queue(WorkQ *queue, Work *work);

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

    /**
     * @brief Set a new work handler.
     *
     * Sets a new work handler as long as the work item is idle and not
     * scheduled or queued.
     *
     * @param[in] handler_ The new work handler.
     * @return 0 on success.
     * @return -EBUSY if the work item is busy.
     */
    int set_new_work_handler(const work_handler_t &handler_);

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
    int resubmit();
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

/**
 * @struct WorkQConfig
 * @brief A structure holding optional configuration items for a work queue.
 *
 * This structure, and values it references, are not retained by WorkQ::start
 */
struct WorkQConfig {
    /**
     * @brief The name to be given to the work queue thread.
     *
     * If left null the thread will not have a name.
     */
    const char *name = "";

    /**
     * Control whether the work queue thread should yield between items.
     *
     * Yielding between items helps guarantee the work queue thread does not
     * starve other threads, including cooperative ones released by a work item.
     * This is the default behavior.
     *
     * Set this to `true` to prevent the work queue thread from yielding between
     * items. This may be appropriate when a sequence of items should complete
     * without yielding control.
     */
    bool no_yield = false;

    /**
     * @brief Control whether the work queue thread should be marked as
     * essential thread.
     */
    bool essential = false;

    /**
     * @brief Controls whether work queue monitors work timeouts.
     *
     * If non-zero, the work queue will monitor the duration of each work item.
     * If the work item handler takes longer than the specified time to execute,
     * the work queue thread will be aborted, and an error will be logged.
     *
     * @note This behavior has not been implemented yet.
     * @todo Implement this.
     */
    std::chrono::milliseconds work_timeout_ms = 0ms;
};

/**
 * @class WorkQ
 * @brief A structure used to submit work.
 */
class WorkQ {
  public:
    /**
     * Constructor.
     */
    WorkQ();

    /**
     * Destructor.
     */
    ~WorkQ();

    /**
     * @brief Submit a work item to a queue.
     *
     * @param[in] work Pointer to the work item.
     *
     * @return 0 if work was already submitted to a queue.
     * @return 1 if work was not submitted and has been queued.
     * @return 2 if work was running and has been queue to the queue that was
     * running it.
     * @return -EBUSY
     * * if work submission was rejected because the work item is cancelling; or
     * * queue is draining; or
     * * queue is plugged.
     * @return -ENODEV if queue has not started.
     */
    int submit(Work *work);

    /**
     * @brief Initializes work queue.
     *
     * This configures the work queue thread and starts it running. This
     * function should not be re-invoked on a queue.
     *
     * @param[in] config Optional configuration parameters. Pass @c nullptr if
     * not required, use the defaults documented in WorkQConfig.
     */
    void start(const WorkQConfig *config);

    /**
     * @brief Run work queue using calling thread.
     *
     * This will run the work queue forever unless stopped by WorkQ::stop().
     * This function should not be invoked from a queue.
     *
     * @param[in] config Optional configuration parameters. Pass @c nullptr if
     * not required, use the defaults documented in WorkQConfig.
     */
    void run(const WorkQConfig *config);

    /**
     * @brief Access the thread ID that animates a work queue.
     *
     * This is necessary to grant a work queue thread access to things the work
     * items it will process are expected to use.
     *
     * @return the thread ID associated with the work queue.
     */
    [[nodiscard]] std::thread::id queue_thread_get() const;

    /**
     * @brief Wait until the work queue has drained, optionally plugging it.
     *
     * This blocks submission to the work queue except when coming from queue
     * thread, and blocks the caller until no more work items are available in
     * the queue.
     *
     * If @p plug is true then submission will continue to be blocked after the
     * drain operation completes until WorkQ::queue_unplug() is invoked.
     *
     * @param[in] plug if `true`, the work queue will continue to block new
     * submissions after all items have drained.
     * @return 1 if the caller had to wait for the drain to complete.
     * @return 0 if call did not have to wait.
     * @return negative error code if wait was interrupted or failed.
     *
     * @note Work items that are delayed are not yet associated with their
     * work queue.  They must be canceled externally if a goal is to ensure the
     * work queue remains empty.  The @p plug feature can be used to prevent
     * delayed items from being submitted after the drain completes.
     */
    int queue_drain(bool plug);

    /**
     * @brief Release a work queue to accept new submissions.
     *
     * This releases the block on new submissions placed when
     * WorkQ::queue_drain() is invoked with the @p plug option enabled.  If this
     * is invoked before the drain completes new items may be submitted as soon
     * as the drain completes.
     *
     * @return 0 if successfully unplugged.
     * @return -EALREADY if the work queue was not plugged.
     */
    int queue_unplug();

    /**
     * @brief Stop the work queue.
     *
     * Stops the work queue thread and ensures that no further work will be
     * processed. This call is blocking and guarantees that the work queue
     * thread has terminated cleanly if successful, no work will be processed
     * past this point.
     *
     * @return 0 if the work queue was stopped.
     * @return -EALREADY if the work queue was not started (or already stopped)
     * @return -EBUSY if the work queue is actively processing work items
     */
    int stop();

    /**
     * @brief Stop the work queue.
     *
     * Stops the work queue thread and ensures that no further work will be
     * processed. This call is blocking and guarantees that the work queue
     * thread has terminated cleanly if successful, no work will be processed
     * past this point.
     *
     * @param[in] timeout Maximum time to wait for the work queue to stop.
     *
     * @return 0 if the work queue was stopped.
     * @return -EALREADY if the work queue was not started (or already stopped)
     * @return -EBUSY if the work queue is actively processing work items
     */
    int stop(const std::chrono::milliseconds &timeout);
#if DELAYABLE_WORK
    int schedule(WorkDelayable *dwork, std::chrono::milliseconds delay);
    int reschedule(WorkDelayable *dwork, std::chrono::milliseconds delay);
#endif // DELAYABLE_WORK

    /**
     * @brief Check if the work queue is plugged.
     * @return `true` if the queue is plugged and not accepting new work items,
     * `false` otherwise.
     */
    [[nodiscard]] bool plugged() const;

    /**
     * @brief Work queue flags
     *
     * Retrieves the current flags bitmask of the work queue.
     *
     * @return The current flags of the work queue.
     *
     * @note This is a live snapshot of state, which may change before the
     * result is checked. Use locks where appropriate.
     */
    [[nodiscard]] uint32_t get_flags() const;

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

#if defined(SYS_WORK_QUEUE)
extern WorkQ sys_work_q;

int work_submit(Work *work);
#if DELAYABLE_WORK
int work_schedule(WorkDelayable *dwork, std::chrono::milliseconds delay);
int work_reschedule(WorkDelayable *dwork, std::chrono::milliseconds delay);
#endif // DELAYABLE_WORK
#endif // SYS_WORK_QUEUE

/** @brief Submit a work item to a queue.
 *
 * @param queue pointer to the work queue on which the item should run.  If
 * NULL the queue from the most recent submission will be used.
 *
 * @param work pointer to the work item.
 *
 * @retval 0 if work was already submitted to a queue
 * @retval 1 if work was not submitted and has been queued to @p queue
 * @retval 2 if work was running and has been queued to the queue that was
 * running it
 * @retval -EBUSY
 * * if work submission was rejected because the work item is cancelling; or
 * * @p queue is draining; or
 * * @p queue is plugged.
 * @retval -EINVAL if @p queue is null and the work item has never been run.
 * @retval -ENODEV if @p queue has not been started.
 */
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
