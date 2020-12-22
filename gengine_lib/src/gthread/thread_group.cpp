#include "stdafx.h"

#include "gthread/thread_group.h"

namespace gthread
{
    thread_group::~thread_group()
    {
        join();
    }

    void thread_group::join()
    {
        for (std::thread& thread : m_threads)
        {
            if(thread.joinable())
                thread.join();
        }
    }
}

