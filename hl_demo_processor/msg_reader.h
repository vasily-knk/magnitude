#pragma once

#include "hl_netmsg.h"
#include "serialization/io_streams.h"

namespace hl_netmsg
{
struct msg_reader
{
    explicit msg_reader(binary::input_stream &is)
        : is(is)
    {}
    
    
    template<typename T>
    struct is_simple;
    
    template<typename T>
    constexpr static bool is_simple_v = is_simple<T>::value;

    template<typename T>
    struct is_simple : std::integral_constant<bool, std::is_arithmetic_v<T> || std::is_enum_v<T>> {};

    template<typename T, size_t N>
    struct is_simple<std::array<T, N>> : std::integral_constant<bool, is_simple_v<T>> {};


    template<typename T>
    using if_simple_t = std::enable_if_t<is_simple_v<T>>;
    template<typename T>
    using if_not_simple_t = std::enable_if_t<!is_simple_v<T>>;

    template<typename T>
    using if_simple_msg_t = std::enable_if_t<hl_netmsg::is_simple_msg_v<T>>;

    template<typename T>
    using if_not_simple_msg_t = std::enable_if_t<!hl_netmsg::is_simple_msg_v<T>>;

public:
    template<typename T>
    void read_msg(T &value, if_simple_msg_t<T> * = nullptr)
    {
        reflect(*this, value);        
    }

    template<typename T>
    void read_msg(T &, if_not_simple_msg_t<T> * = nullptr)
    {
        VerifyMsg(false, "Unsupported msg");
    }

    friend struct single_entry_processor<msg_reader>;

    
private:
    template<typename T>
    void operator()(T & value, char const*, if_simple_t<T> * = nullptr)
    {
        is.read(value);
    }

    void operator()(string &value, char const *);

    template<typename T>
    void operator()(T & value, char const *, if_not_simple_t<T> * = nullptr)
    {
        reflect(*this, value);
    }


private:
    binary::input_stream &is;
};

} // namespace hl_netmsg