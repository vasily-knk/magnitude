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

struct entity_state_t
{
    point_3f origin;
    point_3f mins;
    point_3f maxs;
    uint32_t modelindex = 0;
    
    REFL_INNER(entity_state_t)
        REFL_ENTRY(origin)
        REFL_ENTRY(mins)
        REFL_ENTRY(maxs)
        REFL_ENTRY(modelindex)
    REFL_END()
};


struct msg_disp_t 
    : private msg_reader::context_t
{
    ~msg_disp_t()
    {
        auto const msg_stats = msg_stats_.sorted();
        auto const upd_stats = update_stats_.sorted();
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
        msg_reader p(*is_, *this);
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
        delta_descs_[msg.Name] = desc;

        {
            auto &m = clientdata_mapping_;
            if (!m && msg.Name == "clientdata_t")
                m = boost::in_place(desc);
        }

        {
            auto &m = entity_state_mapping_;
            if (!m && msg.Name == "entity_state_t")
                m = boost::in_place(desc);
        }

        {
            auto &m = entity_state_player_mapping_;
            if (!m && msg.Name == "entity_state_player_t")
                m = boost::in_place(desc);
        }
    }

    void process_msg_impl(msg_t<SVC_SERVERINFO> const &msg)
    {
        max_clients_ = msg.MaxPlayers;
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
        if (is_hltv_)
            return;

        Verify(clientdata_mapping_);
        clientdata_mapping_->apply_delta(clientdata_, msg.client_data);
    }

    void process_msg_impl(msg_t<SVC_SPAWNBASELINE> const &msg)
    {
        for (auto const &e : msg.ents)
        {
            if (e.custom)
                continue;
            
            ents_[e.index] = entity_state_t();

            apply_entity_delta(e.index, e.baseline);
        }
    }

    void process_msg_impl(msg_t<SVC_PACKETENTITIES> const &msg)
    {
        for (auto const &e : msg.ents)
            apply_entity_delta(e.index, e.delta);
    }

    void process_msg_impl(msg_t<SVC_DELTAPACKETENTITIES> const &msg)
    {
        for (auto const &e : msg.ents)
            apply_entity_delta(e.index, e.delta);
    }

    void process_msg_impl(msg_t<SVC_RESOURCELIST> const &msg)
    {
        for (auto const &r : msg.Resources)
        {
            if (r.type == 2)
                models_[r.index] = r.name;
        }
    }
    void process_msg_impl(msg_t<SVC_HLTV> const &)
    {
        is_hltv_ = true;
    }
    
private:
    void apply_entity_delta(uint32_t index, delta_struct_t const &delta)
    {
        auto &state = ents_[index];
        if (is_player(index))
        {
            auto &m = entity_state_player_mapping_;
            Verify(m);
            m->apply_delta(state, delta);
        } 
        else 
        {
            auto &m = entity_state_mapping_;
            Verify(m);
            m->apply_delta(state, delta);
        }
    }


private:
    delta_desc_cptr get_delta_desc(string const& name) const override
    {
        return delta_descs_.at(name);
    }

    delta_desc_cptr get_entity_delta_desc(uint32_t index, bool custom) const override
    {
        if (custom)
            return get_delta_desc("custom_entity_state_t");

        if (is_player(index))
            return get_delta_desc("entity_state_player_t");

        return get_delta_desc("entity_state_t");
    }


public:
    bool is_hltv() const override
    {
        return is_hltv_;
    }

private:
    bool is_player(uint32_t index) const
    {
        Verify(max_clients_);
        return index > 0 && index <= *max_clients_;        
    }

private:
    struct user_msg_t
    {
        optional<size_t> size;
        string name;
    };

private:
    binary::input_stream *is_ = nullptr;

    std::map<string, delta_desc_cptr> delta_descs_;
    optional<uint32_t> max_clients_;
    std::map<uint8_t, user_msg_t> user_msgs_;
    stats_counter_t<msg_type_e> msg_stats_;

    optional<delta_struct_mapping_t<clientdata_t>> clientdata_mapping_;
    optional<delta_struct_mapping_t<entity_state_t>> entity_state_mapping_;
    optional<delta_struct_mapping_t<entity_state_t>> entity_state_player_mapping_;
    clientdata_t clientdata_;

    std::map<uint32_t, entity_state_t> ents_;

    std::ofstream out_ = std::ofstream("aaa.txt");

    stats_counter_t<string> update_stats_;
    std::map<uint32_t, string> models_;
    bool is_hltv_ = false;
};

} // namespace


bool process_demo(fs::path const &filename)
{
    optional<DemoFile> df;

    try
    {
        DemoFile df_cand(filename.c_str(), true);
        df = std::move(df_cand);
    } 
    catch (std::exception const &)
    {
        return false;
    }

    if (df->header.netProtocol < 47)
        return false;


    msg_disp_t disp;

    try
    {

    for (auto const &dir : df->directoryEntries)
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

    }
    catch (std::exception const &)
    {
        return false;
    }
    return true;
}

int amain()
{
    stats_counter_t<int32_t> protocol_stats;
    
    fs::recursive_directory_iterator const beg("E:/csdemos"), end;

    size_t good = 0, bad = 0;
    for (auto it = beg; it != end; ++it)
    {
        if (!fs::is_regular_file(it->path()))
            continue;

        if (it->path().extension() != ".dem")
            continue;

        if (process_demo(it->path()))
            ++good;
        else
            ++bad;

        if ((good + bad) % 10 == 0)
        {
            std::cout << good << ", " << bad << std::endl;
        }
    }

    return 0;
}


int main()
{
    process_demo("data/omg23_duttdutt_limbokungen-de_dust2.dem");
    return 0;
}