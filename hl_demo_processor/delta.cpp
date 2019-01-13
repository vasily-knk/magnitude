#include "delta.h"
#include "bit_reader.h"

namespace hl_netmsg
{

string read_string(binary::bit_reader &br);

namespace
{	
	template<typename T>
	delta_struct_entry_t compose(uint32_t base, uint32_t divisor, bool neg)
	{
		return T(base);
	}

	delta_struct_entry_t read_entry(binary::bit_reader &br, delta_desc_entry_t const &e)
	{
        if (e.flags & DF_String)
            return read_string(br);

		size_t nbits = e.nbits;
		bool neg = false;

		bool const is_signed = (e.flags & DF_Signed) != 0;
		if (is_signed)
		{
			Verify(nbits > 0);
			neg = br.read_bool();
			--nbits;
		}

		uint32_t base = br.read_uint(nbits);
		return base;
		
	}

} // namespace

delta_struct_t delta_decode_struct(binary::bit_reader &br, delta_desc_cptr desc)
{
    auto const bitmask_size_bytes = br.read_bits(3).to_ulong();
    auto const bitmask = br.read_bits(bitmask_size_bytes * 8);

    //Verify(desc.size() >= bitmask.size());
    auto const sz = std::min(desc->size(), bitmask.size());

	delta_struct_t result = {desc};
	result.entries.resize(desc->size());

            
    for (size_t i = 0; i < sz; ++i)
    {
        if (!bitmask[i])
            continue;

        auto const &e = desc->at(i);
		result.entries.at(i) = read_entry(br, e);
    }

	return result;
}

} // namespace hl_netmsg
