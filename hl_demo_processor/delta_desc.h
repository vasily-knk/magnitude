#pragma once

namespace hl_netmsg
{
    
struct delta_entry_t
{
    uint32_t flags;
    string name;
    uint16_t offset;
    uint8_t size;
    uint8_t nbits;
};

    
struct delta_desc_t
{
};

} // namespace hl_netmsg
