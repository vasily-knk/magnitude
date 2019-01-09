#include <iostream>
#include <set>
#include <map>
#include <algorithm>
#include "HLDemoParser/src/DemoFile.hpp"

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

enum msg_type_e
{
SVC_BAD                  = 0 ,
SVC_NOP                  = 1 ,
SVC_DISCONNECT           = 2 ,
SVC_EVENT                = 3 ,
SVC_VERSION              = 4 ,
SVC_SETVIEW              = 5 ,
SVC_SOUND                = 6 ,
SVC_TIME                 = 7 ,
SVC_PRINT                = 8 ,
SVC_STUFFTEXT            = 9 ,
SVC_SETANGLE             = 10,
SVC_SERVERINFO           = 11,
SVC_LIGHTSTYLE           = 12,
SVC_UPDATEUSERINFO       = 13,
SVC_DELTADESCRIPTION     = 14,
SVC_CLIENTDATA           = 15,
SVC_STOPSOUND            = 16,
SVC_PINGS                = 17,
SVC_PARTICLE             = 18,
SVC_DAMAGE               = 19,
SVC_SPAWNSTATIC          = 20,
SVC_EVENT_RELIABLE       = 21,
SVC_SPAWNBASELINE        = 22,
SVC_TEMPENTITY           = 23,
SVC_SETPAUSE             = 24,
SVC_SIGNONNUM            = 25,
SVC_CENTERPRINT          = 26,
SVC_KILLEDMONSTER        = 27,
SVC_FOUNDSECRET          = 28,
SVC_SPAWNSTATICSOUND     = 29,
SVC_INTERMISSION         = 30,
SVC_FINALE               = 31,
SVC_CDTRACK              = 32,
SVC_RESTORE              = 33,
SVC_CUTSCENE             = 34,
SVC_WEAPONANIM           = 35,
SVC_DECALNAME            = 36,
SVC_ROOMTYPE             = 37,
SVC_ADDANGLE             = 38,
SVC_NEWUSERMSG           = 39,
SVC_PACKETENTITIES       = 40,
SVC_DELTAPACKETENTITIES  = 41,
SVC_CHOKE                = 42,
SVC_RESOURCELIST         = 43,
SVC_NEWMOVEVARS          = 44,
SVC_RESOURCEREQUEST      = 45,
SVC_CUSTOMIZATION        = 46,
SVC_CROSSHAIRANGLE       = 47,
SVC_SOUNDFADE            = 48,
SVC_FILETXFERFAILED      = 49,
SVC_HLTV                 = 50,
SVC_DIRECTOR             = 51,
SVC_VOICEINIT            = 52,
SVC_VOICEDATA            = 53,
SVC_SENDEXTRAINFO        = 54,
SVC_TIMESCALE            = 55,
SVC_RESOURCELOCATION     = 56,
SVC_SENDCVARVALUE        = 57,
SVC_SENDCVARVALUE2       = 58,  
};

int main()
{
    DemoFile df("../../../data/mydemo1.dem", true);

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