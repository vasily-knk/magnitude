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

    
struct delta_desc_entry_t
{

    uint32_t flags = 0;
    string name;
    uint16_t offset = 0;
    uint8_t size = 0;
    uint8_t nbits = 0;
    float divisor = 0;
};

typedef vector<delta_desc_entry_t> delta_desc_t;

typedef shared_ptr<delta_desc_t const> delta_desc_cptr;

typedef boost::variant<string, int32_t, uint32_t, float> delta_struct_entry_t;
typedef optional<delta_struct_entry_t> delta_struct_entry_opt_t;


template<typename T>
delta_struct_entry_t make_delta_struct_entry(T const &val)
{
    return val;
}
    
struct delta_struct_t
{
	delta_desc_cptr desc;
	vector<delta_struct_entry_opt_t> entries;

#ifndef NDEBUG
    std::map<string, string> debug_strings;
    std::map<string, int32_t> debug_ints;
    std::map<string, uint32_t> debug_uints;
    std::map<string, float> debug_floats;
#endif
};

delta_struct_t delta_decode_struct(binary::bit_reader &br, delta_desc_cptr desc);

} // namespace hl_netmsg
