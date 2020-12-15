#pragma once

#include "gthread/task.h"
#include "gthread/job_manager.h"

namespace gthread
{
    struct task_group
    {
        task::id add_task(std::function<void()> function);
        void add_parent(task::id child, task::id parent);
       
        void run_tasks(job_manager& job_manager);

    private:

        struct task_info
        {
            std::vector<task::id> m_children;
            std::vector<task::id> m_parents;
        };

        using task_map_info = std::unordered_map<task::id, task_info>;

        void launch_task(job_manager& job_manager, task& task_to_run, task_map_info& infos, std::mutex& task_info_lock, gthread::latch& latch);

        task_map_info m_task_infos;
        std::vector<task> m_task;
        task::id m_id_generator{ 0 };
    };
}