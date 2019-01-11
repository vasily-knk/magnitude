#include "HLDemoParser/src/DemoFile.hpp"

#include "hl_netmsg.h"
#include "serialization/io_streams.h"
#include "msg_reader.h"

using namespace hl_netmsg;



namespace 
{

struct msg_disp_t         
{
    explicit msg_disp_t(binary::input_stream& is)
        : is_(is)
    {
    }

    void go()
    {
        while (!is_.eof())
        {
            msg_type_e id;
            is_.read(reinterpret_cast<uint8_t&>(id));

            process_msg(id);
        }
    }

private:
    void process_msg(msg_type_e id)
    {
        if (id >= SVC_NUM_VALUES)
        {
            LogWarn("Unsupported msg: " << id);
            return;
        }

        process_msg_cand<SVC_BAD>(id);
    }

    template<msg_type_e Id>
    void process_msg_cand(msg_type_e id, std::enable_if_t<Id < SVC_NUM_VALUES> * = nullptr)
    {
        if (Id < id)
            return process_msg_cand<msg_type_e(Id + 1)>(id);

        msg_t<Id> msg;
        msg_reader p(is_);
        p.read_msg(msg);           

        int aaa = 5;
    }

    template<msg_type_e Id>
    void process_msg_cand(msg_type_e, std::enable_if_t<Id == SVC_NUM_VALUES> * = nullptr)
    {
        Verify(false);
    }
    
private:
    binary::input_stream &is_;
};
} // namespace



int main()
{
    
    DemoFile df("data/mydemo1.dem", true);

    for (auto const &dir : df.directoryEntries)
    {
        for (auto const &frame : dir.frames)
        {
            auto const frame_type_id = uint8_t(frame->type);
            if (frame_type_id != 0 && frame_type_id != 1)
                continue;

            auto const *msg_frame = static_cast<NetMsgFrame const *>(frame.get());

            auto const & data = msg_frame->msg;
            binary::input_stream is(data.data(), data.size());
            msg_disp_t disp(is);
            disp.go();

        }
    }


    return 0;
}