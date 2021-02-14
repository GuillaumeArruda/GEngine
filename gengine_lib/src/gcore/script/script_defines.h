#pragma once

namespace gcore
{
    enum class pin_type : char
    {
        input,
        output,
        count
    };

    enum class pin_state : char
    {
        connected,
        disconnected,
    };

    using node_id_t = std::uint32_t;
}