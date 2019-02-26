#pragma once

#include "hl_netmsg.h"
#include "serialization/io_streams.h"
#include "delta.h"


namespace hl_netmsg
{
    struct msg_reader
    {
        struct context_t
        {
            virtual ~context_t() = default;
        	
            virtual delta_desc_cptr get_delta_desc(string const &name) const = 0;
            virtual delta_desc_cptr get_entity_delta_desc(uint32_t index, bool custom) const = 0;
            virtual bool is_hltv() const = 0;
        };

        explicit msg_reader(binary::input_stream& is, context_t const& context)
            : is(is)
            , context_(context)
        {
        }


        template <typename T>
        struct is_simple;

        template <typename T>
        constexpr static bool is_simple_v = is_simple<T>::value;

        template <typename T>
        struct is_simple : std::integral_constant<bool, std::is_arithmetic_v<T> || std::is_enum_v<T>>
        {
        };

        template <typename T, size_t N>
        struct is_simple<std::array<T, N>> : std::integral_constant<bool, is_simple_v<T>>
        {
        };


        template <typename T>
        using if_simple_t = std::enable_if_t<is_simple_v<T>>;
        template <typename T>
        using if_not_simple_t = std::enable_if_t<!is_simple_v<T>>;

        template <typename T>
        using if_simple_msg_t = std::enable_if_t<hl_netmsg::is_simple_msg_v<T>>;

        template <typename T>
        using if_not_simple_msg_t = std::enable_if_t<!hl_netmsg::is_simple_msg_v<T>>;

    public:
        template <msg_type_e Id>
        void read_msg(msg_t<Id>& value, if_simple_msg_t<msg_t<Id>>* = nullptr)
        {
            reflect(*this, value);
        }

        template <msg_type_e Id>
        void read_msg(msg_t<Id>&, if_not_simple_msg_t<msg_t<Id>>* = nullptr)
        {
            auto const id = Id;
            VerifyMsg(false, "Unsupported msg: " << id);
        }

        void read_msg(msg_t<SVC_BAD>& msg);
        void read_msg(msg_t<SVC_DELTADESCRIPTION>& msg);
        void read_msg(msg_t<SVC_RESOURCELIST>& msg);
        void read_msg(msg_t<SVC_SPAWNBASELINE>& msg);
        void read_msg(msg_t<SVC_CLIENTDATA>& msg);
        void read_msg(msg_t<SVC_TEMPENTITY>& msg);
        void read_msg(msg_t<SVC_DIRECTOR>& msg);
        void read_msg(msg_t<SVC_SOUND>& msg);
        void read_msg(msg_t<SVC_EVENT_RELIABLE>& msg);
        void read_msg(msg_t<SVC_EVENT>& msg);
        void read_msg(msg_t<SVC_SPAWNSTATIC>& msg);
        void read_msg(msg_t<SVC_PACKETENTITIES>& msg);
        void read_msg(msg_t<SVC_DELTAPACKETENTITIES>& msg);
        void read_msg(msg_t<SVC_PINGS>& msg);
        void read_msg(msg_t<SVC_SERVERINFO>& msg);
        void read_msg(msg_t<SVC_VOICEDATA>& msg);

    private:
		typedef binary::bit_reader bit_reader;

        bool is_footer(bit_reader const &br) const;
		delta_struct_t read_entity_delta(bit_reader &br, uint32_t entity_number, bool custom);

        typedef uint32_t UInt32;
        typedef uint16_t UInt16;
        typedef bool Boolean;

        friend struct single_entry_processor<msg_reader>;

    private:
        template <typename T>
        void operator()(T& value, char const*)
        {
            read_field(value);
        }

    private:
        template <typename T>
        void read_field(T& value, if_simple_t<T>* = nullptr)
        {
            is.read(value);
        }

        void read_field(string& value);

        template <typename T>
        void read_field(T& value, if_not_simple_t<T>* = nullptr)
        {
            reflect(*this, value);
        }


    private:
        binary::input_stream& is;
        context_t const& context_;
    };
} // namespace hl_netmsg

