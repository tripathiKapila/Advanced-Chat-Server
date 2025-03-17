#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
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
     * @brief Enqueues a task for execution, returning a future for the result.
     * @tparam F A callable object.
     * @param f The task to enqueue.
     * @param args Arguments for the task.
     * @return std::future for the task's return value.
     */
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type> {
        
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
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
