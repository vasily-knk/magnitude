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


namespace detail
{
    struct delta_struct_mapping_base_t
    {
        delta_desc_cptr desc_;
        std::vector<size_t> mapping_;        
    };
    
    
    struct init_processor
    {
        explicit init_processor(delta_struct_mapping_base_t& owner)
            : owner_(owner)
        {
        }

        template<typename T>
        void operator()(T const &, char const *name, ...)
        {
            auto const it = boost::find_if(*owner_.desc_, [name](delta_desc_entry_t const &e){
                return e.name == name;
            });

            Verify(it != owner_.desc_->end());
            
            owner_.mapping_.push_back(it - owner_.desc_->begin());
        }

        template<typename T>
        void operator()(geom::point_t<T, 3> const &p, char const *name_cstr, ...)
        {
            string const name = name_cstr;
            (*this)(p.x, (name + "[0]").c_str());
            (*this)(p.y, (name + "[1]").c_str());
            (*this)(p.z, (name + "[2]").c_str());
        }

    private:
        delta_struct_mapping_base_t &owner_;
    };

    struct apply_processor
    {
        
        explicit apply_processor(delta_struct_mapping_base_t const& owner, delta_struct_t const &delta)
            : owner_(owner)
            , delta_(delta)
        {
        }

        template<typename T>
        void operator()(T &e, ...)
        {
            apply_internal(e);
            ++index_;
        }

        template<typename T>
        void operator()(geom::point_t<T, 3> &p, ...)
        {
            (*this)(p.x);
            (*this)(p.y);
            (*this)(p.z);
        }

    private:
        template<typename T>
        void apply_internal(T &e) const
        {
            auto const delta_index = owner_.mapping_.at(index_);
            if (delta_index >= delta_.entries.size())
                return;

            auto const &delta_val_opt = delta_.entries.at(delta_index);
            if (!delta_val_opt)
                return;

            e = boost::get<T>(*delta_val_opt);
        }

    private:
        delta_struct_mapping_base_t const &owner_;
        delta_struct_t const &delta_;
        size_t index_ = 0;
    };


} // namespace detail

template<typename Struct>
struct delta_struct_mapping_t
    : private detail::delta_struct_mapping_base_t
{
    explicit delta_struct_mapping_t(delta_desc_cptr desc, Struct const &s = Struct())
        : delta_struct_mapping_base_t{desc}
    {
        detail::init_processor proc(*this);
        reflect(proc, s);
    }

    void apply_delta(Struct const &s, delta_struct_t const &delta) const
    {
        Verify(delta.desc == desc_);

        detail::apply_processor proc(*this, delta);
        reflect(proc, s);
    }
};

delta_struct_t delta_decode_struct(binary::bit_reader &br, delta_desc_cptr desc);

} // namespace hl_netmsg
