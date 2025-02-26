#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <utility>

/**
 * @brief A minimal thread pool for executing background tasks.
 */
class ThreadPool {
public:
    /**
     * @brief Constructs a ThreadPool with a fixed number of worker threads.
     * @param numThreads Number of threads to spawn.
     */
    explicit ThreadPool(size_t numThreads);

    /**
     * @brief Enqueues a task for execution.
     * @tparam F A callable object.
     * @param f The task to enqueue.
     */
    template <class F>
    void enqueue(F&& f) {
        std::lock_guard<std::mutex> lock(queueMutex);
        tasks.emplace(std::forward<F>(f));
        condition.notify_one();
    }

    /**
     * @brief Destructor. Joins all threads before destruction.
     */
    ~ThreadPool();

private:
    std::vector<std::thread> workers;               ///< Worker threads.
    std::queue<std::function<void()>> tasks;        ///< Task queue.
    std::mutex queueMutex;                          ///< Mutex for synchronizing access to tasks.
    std::condition_variable condition;              ///< Condition variable for task notification.
    bool stop;                                      ///< Flag to signal thread pool shutdown.
};

#endif // THREADPOOL_HPP
