#pragma once

namespace hl_netmsg
{
    
enum delta_flags_e
{
    DF_Byte = (1 << 0),
    DF_Short = (1 << 1),
    DF_Float = (1 << 2),
    DF_Integer = (1 << 3),
    DF_Angle = (1 << 4),
    DF_TimeWindow8 = (1 << 5),
    DF_TimeWindowBig = (1 << 6),
    DF_String = (1 << 7),
    DF_Signed = (1 << 31)
};

    
struct delta_entry_t
{

    uint32_t flags;
    string name;
    uint16_t offset;
    uint8_t size;
    uint8_t nbits;
};

typedef vector<delta_entry_t> delta_desc_t;

typedef std::map<string, delta_desc_t> delta_desc_map_t;

} // namespace hl_netmsg
