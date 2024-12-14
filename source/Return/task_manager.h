#pragma once

#include "containers.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace re
{
    /* Need two things for easy threading
    *   -Pre-created threads in a thread pool
    *   -An easy way to queue and dispatch tasks to the threads in the pool
    *  These tasks are encapsulated in the TaskManager
    */

    class TaskManager
    {
    public:
        using Task = std::function<void()>;
        using IndexedTask = std::function<void(int)>;

        TaskManager(int thread_count = 0);
        ~TaskManager();

        void init(int thread_count);
        void shutdown();

        //add tasks
        void add_task(Task task);
        void add_tasks(IndexedTask task, int start, int end);

        //execute tasks
        void finish_tasks();

    private:
        void task_thread_loop();
        bool execute_tasks();

        //thread pool
        std::vector<std::thread> m_thread_pool;
        bool m_running = false;

        //tasks - need a list of tasks which can store any number, where threads can take a bunch at a time
        //  circular buffer might make the most sense 
        ExpandingQueue<Task> m_tasks;
        std::mutex m_task_mutex;
        int m_task_batch_count = 1; //how many tasks a thread will take on at a time

        IndexedTask m_task;
        int m_next = 0;
        int m_end = 0;
    };
}