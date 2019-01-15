#include "HLDemoParser/src/DemoFile.hpp"

#include "hl_netmsg.h"
#include "serialization/io_streams.h"
#include "msg_reader.h"
#include "common/stl_helpers.h"
#include "common/stats_counter.h"

using namespace hl_netmsg;



namespace 
{

struct clientdata_t
{
    point_3f origin;
    point_3f velocity;
    
    REFL_INNER(clientdata_t)
        REFL_ENTRY(origin)
        REFL_ENTRY(velocity)
    REFL_END()
};


struct msg_disp_t         
{
    ~msg_disp_t()
    {
        auto const stats = msg_stats_.sorted();
        int aaa = 5;
    }
    
    void go(binary::input_stream& is)
    {
        is_ = &is;
        
        while (!is.eof())
        {
            uint8_t id;
            is.read(id);

            process_msg(id);
        }


        is_ = nullptr;
    }

private:
    void process_msg(uint8_t id)
    {
        auto const id_e = msg_type_e(id);
        
        msg_stats_.add(id_e);
        
        if (auto const *um = stl_helpers::map_item(user_msgs_, id))
        {
            size_t to_skip;
            if (um->size)
            {
                to_skip = *um->size;
            }
            else
            {
                uint8_t val;
                is_->read(val);
                to_skip = val;
            }

            is_->skip(to_skip);
            return;
        }
        
        Verify(id < SVC_NUM_VALUES);

        process_msg_cand<SVC_BAD>(id_e);
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
        auto desc = make_shared<delta_desc_t>(msg.Entries);
        reader_context_.delta_desc_map[msg.Name] = desc;

        if (!clientdata_mapping_ && msg.Name == "clientdata_t")
            clientdata_mapping_ = boost::in_place(desc);
    }

    void process_msg_impl(msg_t<SVC_SERVERINFO> const &msg)
    {
        reader_context_.max_clients = msg.MaxPlayers;
    }    

    void process_msg_impl(msg_t<SVC_NEWUSERMSG> const &msg)
    {
        user_msg_t um;
        if (msg.Size != -1)
        {
            Verify(msg.Size >= 0);
            um.size = msg.Size;
        }

        auto end = boost::find(msg.Name, '\0');
        um.name.assign(msg.Name.begin(), end);

        user_msgs_[msg.Index] = um;
    }    

    void process_msg_impl(msg_t<SVC_CLIENTDATA> const &msg)
    {
        Verify(clientdata_mapping_);
        clientdata_mapping_->apply_delta(clientdata_, msg.client_data);

        out_ 
            << clientdata_.origin.x << "\t"
            << clientdata_.origin.y << "\t"
            << clientdata_.origin.z << "\n"
        ;

    }

private:
    struct user_msg_t
    {
        optional<size_t> size;
        string name;
    };

private:
    binary::input_stream *is_ = nullptr;

    msg_reader::context_t reader_context_;
    std::map<uint8_t, user_msg_t> user_msgs_;
    stats_counter_t<msg_type_e> msg_stats_;

    optional<delta_struct_mapping_t<clientdata_t>> clientdata_mapping_;
    clientdata_t clientdata_;

    std::ofstream out_ = std::ofstream("aaa.txt");
};

} // namespace



int main()
{
    
    DemoFile df("data/mydemo1.dem", true);

        msg_disp_t disp;

    for (auto const &dir : df.directoryEntries)
    {
        int aaa = 5;
        
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