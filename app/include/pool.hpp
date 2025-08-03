#pragma once 

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

/* 
    i've done it 0.5 times before, no harm in trying here again

    two reasons for this

    for one, it's good pracrtice for me, 
    and it's not forbidden by task's specification

    two, while it seems like overkill for the task at hand,
    i feel like this would be an okay solution in a more
    real task
*/

class Pool {

    public:

    Pool();     // will call emplace()
    ~Pool();    // will call stop()

    void emplace();     // will fill the queue of threads with running threads
    
    // will queue the jobs -- more on that below the class declaration
    template<typename F, typename ... Args> 
    void queue(F&& f, Args&&... args);

    void stop();        // will stop this perpetuum mobile
    bool isWorking();   // will check whether there are active jobs

    private:

    // the main course
    void fire();

    std::queue<std::thread> threads;            // running threads
    std::queue<std::function<void()>> jobs;     // queued jobs

    // our lords and saviours
    std::mutex mutex;
    std::condition_variable cv;

    // will be used in fire() to check if threads need to shut down
    bool need_termination = false;
};

template<typename F, typename ... Args> 
void Pool::queue(F&& f, Args&&... args) {
    {
        std::unique_lock<std::mutex> lock(mutex);

        // std::queue<std::function<void()>> jobs; --->
        // so here's the thing, there's a queue of jobs that
        // take no arguments (and return void)

        // so the idea is to wrap everything into lambda which
        // will take no arguments, queue this lambda as a job, and
        // invoke the function inside it later

        jobs.emplace(
            [function = std::forward<F>(f), ... captured = std::forward<Args>(args)] () {
                std::invoke(function, captured...);
            }
        );
    }
    // and then notify someone that there's now a job pending
    cv.notify_one();
}