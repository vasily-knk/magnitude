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

        string read_string(binary::input_stream &is)
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

            return ss.str();            
        }

        float read_coord(bit_reader &br)
        {
            auto const intFlag = br.read_bool();
            auto const fractionFlag = br.read_bool();

            float value = 0.0f;

            if (!intFlag && !fractionFlag)
            {
                return value;
            }

            bool const sign = br.read_bool();
            uint32_t intValue = 0;
            uint32_t fractionValue = 0;

            if (intFlag)
            {
                intValue = br.read_uint(12);
            }

            if (fractionFlag)
            {
                fractionValue = br.read_uint(3);
            }

            value = intValue + ((float)fractionValue * 1.0f / 32.0f);

            if (sign)
            {
                value = -value;
            }

            return value;
        }

        point_3f read_vec3(bit_reader &br)
        {
            point_3f vec;
            
            bool const xflag = br.read_bool();
            bool const yflag = br.read_bool();
            bool const zflag = br.read_bool();

            if (xflag)
                vec.x = read_coord(br);
            if (yflag)
                vec.y = read_coord(br);
            if (zflag)
                vec.z = read_coord(br);

            return vec;
        }
    
    void skip_delta_compressed(bit_reader &br, delta_desc_t const &desc)
    {
        auto const bitmask_size_bytes = br.read_bits(3).to_ulong();
        auto const bitmask = br.read_bits(bitmask_size_bytes * 8);

        //Verify(desc.size() >= bitmask.size());
        auto const sz = std::min(desc.size(), bitmask.size());
            
        for (size_t i = 0; i < sz; ++i)
        {
            if (!bitmask[i])
                continue;

            auto const &e = desc.at(i);
            
            if (e.flags & DF_String)
                read_string(br);
            else
                br.read_bits(e.nbits);
        }
    }


    } // namespace 


    void msg_reader::read_msg(msg_t<SVC_BAD>& msg)
    {
        //Verify(false);
    }

    void msg_reader::read_msg(msg_t<SVC_DELTADESCRIPTION>& msg)
    {
        read_field(msg.Name);
        short num_entries;
        read_field(num_entries);

        bit_reader br(is);
                                     
        msg.Entries.resize(num_entries);

        auto last_entry = context_.last_entry;
        
        for (auto &entry : msg.Entries)
        {
            auto const bitmask_size_bytes = br.read_bits(3).to_ulong();
            auto const bitmask = br.read_bits(bitmask_size_bytes * 8);

            Verify(last_entry || bitmask.to_ulong() == 0x7F);

            if (last_entry)
                entry = *last_entry;

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

            last_entry = entry;
        }

        int aaa = 6;
    }

    void msg_reader::read_msg(msg_t<SVC_RESOURCELIST>& msg)
    {
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

    void msg_reader::read_msg(msg_t<SVC_SPAWNBASELINE>& msg)
    {
        bit_reader br(is);

        while (true)
        {
            auto const entityIndex = br.read_uint(11);

            if (entityIndex == (1<<11)-1) // all 1's
            {
                break;
            }

            auto const entityType = br.read_uint(2);
            char const *entityTypeString = nullptr;

            bool isPlayer = false;
                
            if ((entityType & 1) != 0) // is bit 1 set?
            {
                Verify(context_.max_clients);
                
                if (entityIndex > 0 && entityIndex <= *context_.max_clients)
                {
                    entityTypeString = "entity_state_player_t";
                    isPlayer = true;
                }
                else
                {
                    entityTypeString = "entity_state_t";
                }
            }
            else
            {
                entityTypeString = "custom_entity_state_t";
            }

            skip_delta_compressed(br, context_.delta_desc_map.at(entityTypeString));
        }

        Verify(br.read_uint(5) == (1 << 5) - 1);

        auto const nExtraData = br.read_uint(6);

        for (size_t i = 0; i < nExtraData; i++)
        {
            skip_delta_compressed(br, context_.delta_desc_map.at("entity_state_t"));
        }
    }

    void msg_reader::read_msg(msg_t<SVC_CLIENTDATA>& msg)
    {
        bit_reader br(is);
        
        auto const deltaSequence = br.read_bool();

        if (deltaSequence)
        {
            br.read_uint(8); // delta sequence number
        }

        skip_delta_compressed(br, context_.delta_desc_map.at("clientdata_t"));
            
        while (br.read_bool())
        {
            br.read_uint(6); // weapon index

            skip_delta_compressed(br, context_.delta_desc_map.at("weapon_data_t"));
        }

    }

    void msg_reader::read_msg(msg_t<SVC_TEMPENTITY>& msg)
    {
        uint8_t type;
        is.read(type);


        auto Seek = [this](size_t n)
        {
            is.skip(n);
        };

        if (type == 13)
        {
            Seek(8);

            uint16_t entityIndex;
            is.read(entityIndex);

            if (entityIndex != 0)
                Seek(2);
        }
        else if (type == 29)
        {
            Seek(5);
            uint8_t textParmsEffect;
            is.read(textParmsEffect);
            Seek(14);

            if (textParmsEffect == 2)
            {
                Seek(2);
            }

            auto const str = read_string(is);

            int aaa = 5;
        }
        else
        {

        switch (type)
        {
            // obsolete
            case 16: // TE_BEAM
            case 26: // TE_BEAMHOSE
                break;

            // simple coord format messages
            case 2: // TE_GUNSHOT
            case 4: // TE_TAREXPLOSION 
            case 9: // TE_SPARKS
            case 10: // TE_LAVASPLASH
            case 11: // TE_TELEPORT
                Seek(6);
                break;

            case 0: // TE_BEAMPOINTS
                Seek(24);
                break;

            case 1: // TE_BEAMENTPOINT
                Seek(20);
                break;

            case 3: // TE_EXPLOSION
                Seek(11);
                break;

            case 5: // TE_SMOKE
                Seek(10);
                break;

            case 6: // TE_TRACER
                Seek(12);
                break;

            case 7: // TE_LIGHTNING 
                Seek(17);
                break;

            case 8: // TE_BEAMENTS
                Seek(16);
                break;

            case 12: // TE_EXPLOSION2
                Seek(8);
                break;

            case 13: // TE_BSPDECAL

                break;

            case 14: // TE_IMPLOSION
                Seek(9);
                break;

            case 15: // TE_SPRITETRAIL
                Seek(19);
                break;

            case 17: // TE_SPRITE
                Seek(10);
                break;

            case 18: // TE_BEAMSPRITE
                Seek(16);
                break;

            case 19: // TE_BEAMTORUS
            case 20: // TE_BEAMDISK
            case 21: // TE_BEAMCYLINDER
                Seek(24);
                break;

            case 22: // TE_BEAMFOLLOW
                Seek(10);
                break;

            case 23: // TE_GLOWSPRITE
                // SDK is wrong
                /* 
                    write_coord()	 position
                    write_coord()
                    write_coord()
                    write_short()	 model index
                    write_byte()	 life in 0.1's
                    write_byte()	scale in 0.1's
                    write_byte()	brightness
                */
                Seek(11);
                break;

            case 24: // TE_BEAMRING
                Seek(16);
                break;

            case 25: // TE_STREAK_SPLASH
                Seek(19);
                break;

            case 27: // TE_DLIGHT
                Seek(12);
                break;

            case 28: // TE_ELIGHT
                Seek(16);
                break;

            case 30: // TE_LINE
            case 31: // TE_BOX
                Seek(17);
                break;

            case 99: // TE_KILLBEAM
                Seek(2);
                break;

            case 100: // TE_LARGEFUNNEL
                Seek(10);
                break;

            case 101: // TE_BLOODSTREAM
                Seek(14);
                break;

            case 102: // TE_SHOWLINE
                Seek(12);
                break;

            case 103: // TE_BLOOD
                Seek(14);
                break;

            case 104: // TE_DECAL
                Seek(9);
                break;

            case 105: // TE_FIZZ
                Seek(5);
                break;

            case 106: // TE_MODEL
                // WRITE_ANGLE could be a short..
                Seek(17);
                break;

            case 107: // TE_EXPLODEMODEL
                Seek(13);
                break;

            case 108: // TE_BREAKMODEL
                Seek(24);
                break;

            case 109: // TE_GUNSHOTDECAL
                Seek(9);
                break;

            case 110: // TE_SPRITE_SPRAY
                Seek(17);
                break;

            case 111: // TE_ARMOR_RICOCHET
                Seek(7);
                break;

            case 112: // TE_PLAYERDECAL (could be a trailing short after this, apparently...)
                Seek(10);
                break;

            case 113: // TE_BUBBLES
            case 114: // TE_BUBBLETRAIL
                Seek(19);
                break;

            case 115: // TE_BLOODSPRITE
                Seek(12);
                break;

            case 116: // TE_WORLDDECAL
            case 117: // TE_WORLDDECALHIGH
                Seek(7);
                break;

            case 118: // TE_DECALHIGH
                Seek(9);
                break;

            case 119: // TE_PROJECTILE
                Seek(16);
                break;

            case 120: // TE_SPRAY
                Seek(18);
                break;

            case 121: // TE_PLAYERSPRITES
                Seek(5);
                break;

            case 122: // TE_PARTICLEBURST
                Seek(10);
                break;

            case 123: // TE_FIREFIELD
                Seek(9);
                break;

            case 124: // TE_PLAYERATTACHMENT
                Seek(7);
                break;

            case 125: // TE_KILLPLAYERATTACHMENTS
                Seek(1);
                break;

            case 126: // TE_MULTIGUNSHOT
                Seek(18);
                break;

            case 127: // TE_USERTRACER
                Seek(15);
                break;

            default:
               VerifyMsg(false, "Unknown tempentity: " << type);
        }
        }

    }

    void msg_reader::read_msg(msg_t<SVC_DIRECTOR>& msg)
    {
        uint8_t length;
        is.read(length);

        vector<char> str(length, '\0');
        is.read(str.data(), length);

        int aaa = 5;

    }

    void msg_reader::read_msg(msg_t<SVC_SOUND>& msg)
    {
        bit_reader br(is);

        auto const flags = br.read_uint(9);

        if ((flags & (1 << 0)) != 0) // volume
        {
            br.read_bits(8);
        }

        if ((flags & (1 << 1)) != 0) // attenuation * 64
        {
            br.read_bits(8);
        }

        br.read_bits(3); // channel
        br.read_bits(11); // edict number

        if ((flags & (1 << 2)) != 0) // sound index (short)
        {
            br.read_bits(16);
        }
        else // sound index (byte)
        {
            br.read_bits(8);
        }

        auto const pos = read_vec3(br);

        if ((flags & (1 << 3)) != 0) // pitch
        {
            br.read_bits(8);
        }
    }

    void msg_reader::read_msg(msg_t<SVC_EVENT_RELIABLE>& msg)
    {
        bit_reader br(is);

        br.read_uint(10); // event index

        skip_delta_compressed(br, context_.delta_desc_map.at("event_t"));

        auto const delayBit = br.read_bool();

        if (delayBit)
        {
            br.read_uint(16); // delay / 100.0f
        }
    }

    void msg_reader::read_msg(msg_t<SVC_EVENT>& msg)
    {
        bit_reader br(is);
        auto const nEvents = br.read_uint(5);

        for (size_t i = 0; i < nEvents; i++)
        {
            br.read_uint(10); // event index

            bool const packetIndexBit = br.read_bool();

            if (packetIndexBit)
            {
                br.read_uint(11); // packet index

                bool const deltaBit = br.read_bool();

                if (deltaBit)
                {
                    skip_delta_compressed(br, context_.delta_desc_map.at("event_t"));
                }
            }

            bool const fireTimeBit = br.read_bool();

            if (fireTimeBit)
            {
                br.read_uint(16); // fire time
            }
        }   
    }

    void msg_reader::read_msg(msg_t<SVC_SPAWNSTATIC>& msg)
    {
        auto &base = static_cast<msg_spawnstatic_base_t&>(msg);
        read_field(base);

        if (base.RenderMode != 0)
        {
            msg.RenderParams = boost::in_place();
            read_field(*msg.RenderParams);
        }

        int aaa = 5;
    }

    void msg_reader::read_msg(msg_t<SVC_PACKETENTITIES>& msg)
    {
        uint16_t num_ents;
        is.read(num_ents);

        UInt32 entityNumber = 0;

        bit_reader br(is);

        // begin entity parsing
        while (true)
        {
            if (is_footer(br))
            {
                br.read_bits(16);
                break;
            }

            Boolean entityNumberIncrement = br.read_bool();

            if (!entityNumberIncrement) // entity number isn't last entity number + 1, need to read it in
            {
                // is the following entity number absolute, or relative from the last one?
                Boolean absoluteEntityNumber = br.read_bool();

                if (absoluteEntityNumber)
                {
                    entityNumber = br.read_uint(11);
                }
                else
                {
                    entityNumber += br.read_uint(6);
                }
            }
            else
            {
                entityNumber++;
            }

            Boolean custom = br.read_bool();
            Boolean useBaseline = br.read_bool();

            if (useBaseline)
            {
                br.read_bits(6); // baseline index
            }

            char const *entityType = "entity_state_t";

            Verify(context_.max_clients);
            if (entityNumber > 0 && entityNumber <= *context_.max_clients)
            {
                entityType = "entity_state_player_t";
            }
            else if (custom)
            {
                entityType = "custom_entity_state_t";
            }

            skip_delta_compressed(br, context_.delta_desc_map.at(entityType));

        }

    }

    void msg_reader::read_msg(msg_t<SVC_DELTAPACKETENTITIES>& msg)
    {
        uint16_t num_ents;
        is.read(num_ents);
        is.skip(1);

        UInt32 entityNumber = 0;

        bit_reader br(is);

        while (true)
        {
            if (is_footer(br))
            {
                br.read_bits(16);
                break;
            }

            Boolean removeEntity = br.read_bool();

            // is the following entity number absolute, or relative from the last one?
            Boolean absoluteEntityNumber = br.read_bool();

            if (absoluteEntityNumber)
            {
                entityNumber = br.read_uint(11);
            }
            else
            {
                entityNumber += br.read_uint(6);
            }

            if (!removeEntity)
            {
                Boolean custom = br.read_bool();

                char const *entityType = "entity_state_t";

                Verify(context_.max_clients);
                if (entityNumber > 0 && entityNumber <= *context_.max_clients)
                {
                    entityType = "entity_state_player_t";
                }
                else if (custom)
                {
                    entityType = "custom_entity_state_t";
                }

                skip_delta_compressed(br, context_.delta_desc_map.at(entityType));

            }
        }

    }

    bool msg_reader::is_footer(bit_reader const& br) const
    {
        auto const rem = br.remaining();
        auto const rem_size = rem.size();
            
        Verify(rem_size < 8);
        if (rem.to_ulong() == 0)
        {
            binary::input_stream fake_is(is.curr(), is.left());
            bit_reader fake_br(fake_is);

            auto const bits_left = 16 - rem_size;
            if (fake_br.read_bits(bits_left).to_ulong() == 0)
                return true;
        }
        return false;
    }

    void msg_reader::read_field(string& value)
    {
        value = read_string(is);
    }
} // namespace hl_netmsg
