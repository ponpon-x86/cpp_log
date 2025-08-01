#include "pool.hpp"

// construction and destruction

Pool::Pool() {
    this->emplace();
}

Pool::~Pool() {
    this->stop();
}

// methods

void Pool::emplace() {
    // get a number of threads the system supports
    auto thread_total = std::thread::hardware_concurrency();
    // shoot our shots
    for(auto i = 0; i < thread_total; ++i)
        threads.emplace(std::thread(&Pool::fire, this));
}

bool Pool::isWorking() {
    bool is_working;
    {
        std::unique_lock<std::mutex> lock(mutex);
        // if there are no jobs queued, persume they are about to be done
        is_working = jobs.empty() ? false : true;
    }
    return is_working;
}

void Pool::stop() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        need_termination = true;
    }

    cv.notify_all();

    while(!threads.empty()) {
        auto& thread = threads.front();
        thread.join(); // join before pop
        threads.pop();
    }
}

// this is where the fun begins
void Pool::fire() {
    // i want each thread to run infinitely, so
    while(true) {
        // this will take and do the job later:
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(mutex);
            // now to lock the thread down until note;
            // i'll use lambda for the predicate
            cv.wait(lock, [this] {
                return need_termination || !jobs.empty();
            });

            // basically,
            // if the pool was stopped, stop the threads
            if (need_termination) return;

            // now, take the job, ...
            job = jobs.front();
            jobs.pop(); // ... remove the taken job from queue ...
        }
        job(); // ... and execute
    }
}