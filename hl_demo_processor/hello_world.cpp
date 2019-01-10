#include <iostream>
#include <set>
#include <map>
#include <algorithm>
#include "HLDemoParser/src/DemoFile.hpp"

#include <cstdint>
#include "hl_netmsg.h"

template<typename T>
struct stats_counter_t
{
    typedef std::pair<T, int> sorted_value_t;
    typedef std::vector<sorted_value_t> sorted_t;

    void add(T const &value)
    {
        auto it = map_.find(value);
        if (it == map_.end())
            it = map_.emplace(value, 0).first;

        ++it->second;
    }

    sorted_t sorted() const
    {
        sorted_t result;

        std::copy(map_.begin(), map_.end(), std::back_inserter(result));

        std::sort(result.begin(), result.end(), [](sorted_value_t const &a, sorted_value_t const &b)
        {
            return a.second > b.second;
        });

        return result;
    }

private:
    std::map<T, int> map_;
};

void print_undef(hl_netmsg::msg_type_e id)
{
               std::cout << "Undef: " << id << std::endl; 
}

template<hl_netmsg::msg_type_e N>
std::enable_if_t<N == hl_netmsg::SVC_NUM_VALUES, void> inst()
{

}

template<hl_netmsg::msg_type_e N>
std::enable_if_t<N < hl_netmsg::SVC_NUM_VALUES, void> inst()
{
    if (!hl_netmsg::msg_t<N>::ok)
        print_undef(N);

    inst<hl_netmsg::msg_type_e(N + 1)>();
}

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