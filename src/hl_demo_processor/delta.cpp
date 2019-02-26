#include "delta.h"
#include "bit_reader.h"

namespace hl_netmsg
{

string read_string(binary::bit_reader &br);

namespace
{	
	template<typename T>
	delta_struct_entry_t compose(uint32_t base, float divisor, bool neg)
	{
		Verify(std::is_signed_v<T> || !neg);
	    auto const val = T(base) / T(divisor) * (neg ? -1 : 1);

        static_assert(std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<decltype(val)>>, "AAA");

        return make_delta_struct_entry(val);
	}

	delta_struct_entry_t read_entry(binary::bit_reader &br, delta_desc_entry_t const &e)
	{
        if (e.flags & DF_String)
            return make_delta_struct_entry(read_string(br));

        size_t nbits = e.nbits;
		bool neg = false;

		bool const is_signed = (e.flags & DF_Signed) != 0;
		if (is_signed)
		{
			Verify(nbits > 0);
			neg = br.read_bool();
			--nbits;
		}

		uint32_t const base = br.read_uint(nbits);

        std::function<delta_struct_entry_t(uint32_t, float, bool)> compose_f;

        Verify(!(e.flags & DF_Angle) || !is_signed);

        float const divisor = (e.flags & DF_Angle) ? (float(1ul << nbits) / 360.f) : e.divisor;
	    
	    if (e.flags & DF_Byte || e.flags & DF_Short || e.flags & DF_Integer)
            compose_f = is_signed ? compose<int32_t> : compose<uint32_t>;
        else if (e.flags & DF_Float || e.flags & DF_TimeWindow8 || e.flags & DF_TimeWindowBig || e.flags & DF_Angle)
            compose_f = compose<float>;

        Verify(compose_f);

		return compose_f(base, divisor, neg);
		
	}

} // namespace

delta_struct_t delta_decode_struct(binary::bit_reader &br, delta_desc_cptr desc)
{
    auto const bitmask_size_bytes = br.read_bits(3).to_ulong();
    auto const bitmask = br.read_bits(bitmask_size_bytes * 8);

    //Verify(desc.size() >= bitmask.size());
    auto const sz = std::min(desc->entries.size(), bitmask.size());

	delta_struct_t result = {desc};
	result.entries.resize(desc->entries.size());

    for (size_t i = 0; i < sz; ++i)
    {
        if (!bitmask[i])
            continue;

        auto const &e = desc->entries.at(i);
		auto const val = read_entry(br, e);
        result.entries.at(i) = val;

#ifdef DEBUG_DELTA_STRUCT
        if (auto const *cval = boost::get<string>(&val))
            result.debug_strings[e.name] = *cval;
        if (auto const *cval = boost::get<int32_t>(&val))
            result.debug_ints[e.name] = *cval;
        if (auto const *cval = boost::get<uint32_t>(&val))
            result.debug_uints[e.name] = *cval;
        if (auto const *cval = boost::get<float>(&val))
            result.debug_floats[e.name] = *cval;
#endif
    }






	return result;
}

} // namespace hl_netmsg
