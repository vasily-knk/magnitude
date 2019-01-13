#include "delta.h"
#include "bit_reader.h"

namespace hl_netmsg
{

string read_string(binary::bit_reader &br);

namespace
{
	template<typename T>
	boost::any compose(uint32_t base, delta_desc_entry_t const& e)
	{
	    return T(base);
	}
} // namespace

delta_struct_t delta_decode_struct(binary::bit_reader &br, delta_desc_cptr desc)
{
    auto const bitmask_size_bytes = br.read_bits(3).to_ulong();
    auto const bitmask = br.read_bits(bitmask_size_bytes * 8);

    //Verify(desc.size() >= bitmask.size());
    auto const sz = std::min(desc->size(), bitmask.size());

	delta_struct_t result = {desc, vector<boost::any>(desc->size())};

            
    for (size_t i = 0; i < sz; ++i)
    {
        if (!bitmask[i])
            continue;

        auto const &e = desc->at(i);
		auto &result_entry = result.entries.at(i);
            
        if (e.flags & DF_String)
		{
            result_entry = read_string(br);
			continue;
		}

		size_t nbits = e.nbits;
		bool neg = false;
		if (e.flags & DF_Signed)
		{
			Verify(nbits > 0);
			neg = br.read_bool();
			--nbits;
		}

		uint32_t base = br.read_uint(nbits);
    }

	return result;
}

} // namespace hl_netmsg
