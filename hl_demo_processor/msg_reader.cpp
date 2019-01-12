#include "msg_reader.h"
#include "bit_reader.h"
#include "delta_desc.h"

namespace hl_netmsg
{
    namespace 
    {
        
        string read_string(bit_reader &br)
        {
            std::stringstream ss;
            while (true)
            {
                auto const c = char(br.read_bits(8).to_ulong());
                if (c == '\0')
                    break;
                ss << c;
            }

            return ss.str();            
        }


    } // namespace 


    void msg_reader::read_msg(msg_t<SVC_DELTADESCRIPTION>& msg)
    {
        read_field(msg.Name);
        short num_entries;
        read_field(num_entries);

        bit_reader br(is);
                                     
        msg.Entries.resize(num_entries);

        for (auto &entry : msg.Entries)
        {
            auto const bitmask_size_bytes = br.read_bits(3).to_ulong();
            auto const bitmask = br.read_bits(bitmask_size_bytes * 8);

            Verify(last_entry_ || bitmask.to_ulong() == 0x7F);

            if (last_entry_)
                entry = *last_entry_;

            if (bitmask[0])
                entry.flags = br.read_bits(32).to_ulong();

            if (bitmask[1])
                entry.name = read_string(br);

            if (bitmask[2])
                entry.offset = br.read_bits(16).to_ulong();
            if (bitmask[3])
                entry.size = br.read_bits(8).to_ulong();
            if (bitmask[4])
                entry.nbits = br.read_bits(8).to_ulong();
            if (bitmask[4])
                br.read_bits(32);
            if (bitmask[6])
                br.read_bits(32);

            last_entry_ = entry;
        }

        int aaa = 6;
    }

    void msg_reader::read_msg(msg_t<SVC_RESOURCELIST>& msg)
    {
        //12 bits 	NumResources
        //4 bits 	Type
        //n bits 	Name
        //12 bits 	Index
        //24 bits 	DownloadSize
        //3 bits 	Flags
        //128 bits 	MD5Hash
        //1 bit 	HasExtraInfo
        //] 256 bits 	ExtraInfo 

        bit_reader br(is);

        auto const NumResources = br.read_uint(12);

        msg.Resources.resize(NumResources);

        for (auto &res : msg.Resources)
        {

            br.read_uint(4, res.type);

            res.name = read_string(br);

            br.read_uint(12, res.index);
            br.read_uint(24, res.size );
            br.read_uint( 3, res.flags);

            if (res.flags & 4)
                br.read_bits(128);

            br.read_uint(1, res.has_extra_info);
            if (res.has_extra_info)
                br.read_bits(128);

        }

        if (br.read_uint(1))
        {
            while (br.read_uint(1))
            {
                auto const read5 = br.read_uint(1);
                auto const num_bits = read5 ? 5 : 10;
                br.read_bits(num_bits);
            }
        }


    }

    void msg_reader::read_field(string& value)
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
} // namespace hl_netmsg
