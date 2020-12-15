#pragma once
#include <functional>

namespace gthread
{
    struct task
    {
        using id = std::uint64_t;
        task(id task_id, std::function<void()> task_function) : m_id(task_id), m_task(std::move(task_function)) {}
        ~task() = default;
        task(const task&) = delete;
        task(task&&) = default;
        task& operator=(const task&) = delete;
        task& operator=(task&&) = default;

        void operator()() { m_task(); }
        id get_id() const { return m_id; }

        bool operator==(id task_id) const noexcept { return m_id == task_id; }

    private:
        std::function<void()> m_task;
        id m_id;
    };
}

