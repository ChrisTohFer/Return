#include "task_manager.h"

#include <algorithm>
#include <cassert>

namespace re
{
    TaskManager::TaskManager(int thread_count)
    {
        if (thread_count > 0)
        {
            init(thread_count);
        }
    }

    TaskManager::~TaskManager()
    {
        shutdown();
    }

    void TaskManager::init(int thread_count)
    {
        assert(!m_running);

        m_running = true;
        m_thread_pool.reserve(thread_count);
        for (int i = 0; i < thread_count; ++i)
        {
            m_thread_pool.push_back(std::thread(&TaskManager::task_thread_loop, this));
        }
    }

    void TaskManager::shutdown()
    {
        m_running = false;
        for (auto& thread : m_thread_pool)
        {
            thread.join();
        }
    }

    void TaskManager::add_task(Task task)
    {
        m_task_mutex.lock();
        m_tasks.push(std::move(task));
        m_task_mutex.unlock();
    }

    void TaskManager::add_tasks(IndexedTask task, int start, int end)
    {
        //m_task_mutex.lock();
        //for (int i = start; i < end; ++i)
        //{
        //    m_tasks.push([task, i]() { task(i); });
        //}
        //m_task_mutex.unlock();

        m_task = std::move(task);
        m_next = start;
        m_end = end;
    }

    void TaskManager::finish_tasks()
    {
        while (m_running && execute_tasks())
        {}
    }

    void TaskManager::task_thread_loop()
    {
        while (m_running)
        {
            //take on a task. if none available, sleep for a bit
            if (!execute_tasks())
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(1us);
            }
        }
    }
    bool TaskManager::execute_tasks()
    {
        int start, end;
        m_task_mutex.lock();
        start = m_next;
        end = std::min(m_end, start + m_task_batch_count);
        m_next = end;
        m_task_mutex.unlock();

        if (start == end)
        {
            return false;
        }
        for (int i = start; i < end; ++i)
        {
            m_task(i);
        }
        return true;

        ////thread local to avoid re-allocating every time
        ////could be part of the manager?
        //thread_local std::vector<Task> local_tasks;
        //
        ////lock mutex and collect tasks
        //m_task_mutex.lock();
        //for (int i = 0; i < m_task_batch_count; ++i)
        //{
        //    if (m_tasks.count() == 0)
        //    {
        //        break;
        //    }
        //
        //    local_tasks.push_back(m_tasks.pop());
        //}
        //m_task_mutex.unlock();
        //
        ////return false if we have no tasks to do
        //if (local_tasks.empty())
        //{
        //    return false;
        //}
        //
        ////run the tasks after unlocking the mutex
        //for (auto& task : local_tasks)
        //{
        //    task();
        //}
        //
        ////clear task list for next iteration
        //local_tasks.clear();
        //
        //return true;
    }
}