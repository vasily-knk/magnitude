#include "HLDemoParser/src/DemoFile.hpp"

#include "hl_netmsg.h"
#include "serialization/io_streams.h"
#include "msg_reader.h"

using namespace hl_netmsg;



namespace 
{

struct msg_disp_t         
{
    void go(binary::input_stream& is)
    {
        is_ = &is;
        
        while (!is.eof())
        {
            msg_type_e id;
            is.read(reinterpret_cast<uint8_t&>(id));

            process_msg(id);
        }

        is_ = nullptr;
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
        msg_reader p(*is_, reader_context_);
        p.read_msg(msg);           

        process_msg_impl(msg);

        int aaa = 5;
    }

    template<msg_type_e Id>
    void process_msg_cand(msg_type_e, std::enable_if_t<Id == SVC_NUM_VALUES> * = nullptr)
    {
        Verify(false);
    }

    template<msg_type_e Id>
    void process_msg_impl(msg_t<Id> const &)
    {
        
    }

    void process_msg_impl(msg_t<SVC_DELTADESCRIPTION> const &msg)
    {
        if (!msg.Entries.empty())
            reader_context_.last_entry = msg.Entries.back();

        reader_context_.delta_desc_map[msg.Name] = msg.Entries;
    }

    void process_msg_impl(msg_t<SVC_SERVERINFO> const &msg)
    {
        reader_context_.max_clients = msg.MaxPlayers;
    }    

private:
    binary::input_stream *is_ = nullptr;

    msg_reader::context_t reader_context_;
};

} // namespace



int main()
{
    
    DemoFile df("data/mydemo1.dem", true);

        msg_disp_t disp;

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
            disp.go(is);

        }
    }


    return 0;
}