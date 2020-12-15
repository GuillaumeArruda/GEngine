#include "stdafx.h"

#include "gthread/thread_group.h"

namespace gthread
{
    thread_group::~thread_group()
    {
        for (std::thread& thread : m_threads)
        {
            thread.join();
        }
    }
}

