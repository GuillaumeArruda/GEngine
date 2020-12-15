#include "stdafx.h"
#include "gthread/task_group.h"
#include "gthread/latch.h"

#include <algorithm>

namespace gthread
{
    task::id gthread::task_group::add_task(std::function<void()> function)
    {
        task::id newId = m_id_generator;
        m_task.emplace_back(m_id_generator++, std::move(function));
        return newId;
    }

    void gthread::task_group::add_parent(task::id child, task::id parent)
    {
        m_task_infos[child].m_parents.push_back(parent);
        m_task_infos[parent].m_children.push_back(child);
    }
    void task_group::launch_task(job_manager& job_manager, task& task_to_run, task_map_info& infos, std::mutex& task_info_lock, gthread::latch& latch)
    {
        job_manager.submit([&]
            {
                task_to_run();
                {
                    std::scoped_lock lock{ task_info_lock };
                    task_info& info = infos[task_to_run.get_id()];
                    for (task::id child : info.m_children)
                    {
                        std::vector<task::id>& parents = infos[child].m_parents;
                        parents.erase(std::find(parents.begin(), parents.end(), task_to_run.get_id()));
                        if (parents.size() == 0)
                            launch_task(job_manager, *std::find(m_task.begin(), m_task.end(), child), infos, task_info_lock, latch);
                    }
                }
                latch.count_down();
            }
        );
    }
    void task_group::run_tasks(job_manager& job_manager)
    {
        std::mutex task_info_lock;
        task_map_info task_info(m_task_infos);
        gthread::latch latch(static_cast<int>(m_task.size()));
        
        {
            std::scoped_lock lock{ task_info_lock };
            for (auto& task : m_task)
            {
                if (task_info[task.get_id()].m_parents.size() == 0)
                {
                    launch_task(job_manager, task, task_info, task_info_lock, latch);
                }
            }
        }
        if (job_manager.is_currently_on_job_thread())
        {
            while (!latch.is_ready())
            {
                job_manager.try_run_job();
            }
        }
        latch.wait();
    }
}
