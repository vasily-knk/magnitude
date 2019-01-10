#include "HLDemoParser/src/DemoFile.hpp"

#include "hl_netmsg.h"
#include "serialization/io_streams.h"

struct proc
{
    explicit proc(binary::input_stream &is)
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
    void operator()(T & value, char const*, if_simple_t<T> * = nullptr)
    {
        is.read(value);
    }

    void operator()(string &value, char const *)
    {
        std::stringstream ss;
        
        while (true)
        {
            char c;
            is.read(c);

            if (c == '\0')
                break;

            ss << c;
        }
        
        value = ss.str();
    }

    template<typename T>
    void operator()(T & value, char const *, if_not_simple_t<T> * = nullptr)
    {
        reflect(*this, value);
    }

private:
    binary::input_stream &is;
};

namespace 
{

using namespace hl_netmsg;

template<msg_type_e N>
std::enable_if_t<N == SVC_NUM_VALUES, void> inst()
{

}

template<msg_type_e N>
std::enable_if_t<!is_simple_msg_v<msg_t<N>>, void> inst_inner()
{
    std::cout << "Undef: " << N << std::endl; 
   
}

template<msg_type_e N>
std::enable_if_t<is_simple_msg_v<msg_t<N>>, void> inst_inner()
{
    std::string bytes = "Hello";
    bytes.resize(128, '\0');

    binary::input_stream is(bytes.data(), bytes.size());
    proc p(is);
    msg_t<N> msg;
    reflect(p, msg);   
}


template<msg_type_e N>
std::enable_if_t<N < SVC_NUM_VALUES, void> inst()
{
    inst_inner<N>();

    inst<msg_type_e(N + 1)>();
}

} // namespace

int main()
{
    
    inst<hl_netmsg::SVC_BAD>();
    
    DemoFile df("data/mydemo1.dem", true);

    for (auto const &dir : df.directoryEntries)
    {
        for (auto const &frame : dir.frames)
        {
            auto const frame_type_id = uint8_t(frame->type);
            if (frame_type_id != 0 && frame_type_id != 1)
                continue;

            auto const *msg_frame = static_cast<NetMsgFrame const *>(frame.get());
        }
    }


    return 0;
}