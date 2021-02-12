#pragma once

namespace gcore
{
    enum class pin_type : char
    {
        input,
        output,
        count
    };

    using node_id_t = std::uint32_t;
}