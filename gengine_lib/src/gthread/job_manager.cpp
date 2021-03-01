#include "stdafx.h"

#include <iostream>
#include "gthread/job_manager.h"

#include <fmt/format.h>
#include <optick/optick.h>

namespace gthread
{
    job_manager::job_manager(unsigned int number_of_thread) 
        : m_job_queues(number_of_thread)
        ,  m_threads(number_of_thread, [&](unsigned index) { OPTICK_THREAD(fmt::format("Worker thread: {}", index).c_str()); process_job(index); }) 
    {}

    job_manager::~job_manager()
    {
        for (auto& queue : m_job_queues)
            queue.done();
    }

    void job_manager::try_run_job()
    {
        try
        {
            std::function<void()> job;
            for (std::size_t i = 0; i < m_job_queues.size(); ++i)
                if (m_job_queues[(i) % m_job_queues.size()].try_dequeue(job)) break;

            if (!job) return;
            job();
        }
        catch (...)
        {
            std::cerr << "Uncatched excetion\n";
        }
    }

    void job_manager::stop_and_join()
    {
        for (auto& queue : m_job_queues)
            queue.done();
        m_threads.join();
    }

    void job_manager::process_job(unsigned index)
    {
        while (true)
        {
            try
            {
                std::function<void()> job;
                for (std::size_t i = 0; i < m_job_queues.size(); ++i)
                    if (m_job_queues[(i + index) % m_job_queues.size()].try_dequeue(job)) break;

                if (!job && !m_job_queues[index].dequeue(job)) break;
                job();
            }
            catch (...)
            {
                std::cerr << "Uncatched excetion\n";
            }
        }
    }
}