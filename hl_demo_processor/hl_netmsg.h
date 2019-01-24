#pragma once
#include "reflection/reflection.h"
#include "delta.h"

namespace hl_netmsg
{
    enum msg_type_e : uint8_t
    {
        SVC_BAD = 0,
        SVC_NOP = 1,
        SVC_DISCONNECT = 2,
        SVC_EVENT = 3,
        SVC_VERSION = 4,
        SVC_SETVIEW = 5,
        SVC_SOUND = 6,
        SVC_TIME = 7,
        SVC_PRINT = 8,
        SVC_STUFFTEXT = 9,
        SVC_SETANGLE = 10,
        SVC_SERVERINFO = 11,
        SVC_LIGHTSTYLE = 12,
        SVC_UPDATEUSERINFO = 13,
        SVC_DELTADESCRIPTION = 14,
        SVC_CLIENTDATA = 15,
        SVC_STOPSOUND = 16,
        SVC_PINGS = 17,
        SVC_PARTICLE = 18,
        SVC_DAMAGE = 19,
        SVC_SPAWNSTATIC = 20,
        SVC_EVENT_RELIABLE = 21,
        SVC_SPAWNBASELINE = 22,
        SVC_TEMPENTITY = 23,
        SVC_SETPAUSE = 24,
        SVC_SIGNONNUM = 25,
        SVC_CENTERPRINT = 26,
        SVC_KILLEDMONSTER = 27,
        SVC_FOUNDSECRET = 28,
        SVC_SPAWNSTATICSOUND = 29,
        SVC_INTERMISSION = 30,
        SVC_FINALE = 31,
        SVC_CDTRACK = 32,
        SVC_RESTORE = 33,
        SVC_CUTSCENE = 34,
        SVC_WEAPONANIM = 35,
        SVC_DECALNAME = 36,
        SVC_ROOMTYPE = 37,
        SVC_ADDANGLE = 38,
        SVC_NEWUSERMSG = 39,
        SVC_PACKETENTITIES = 40,
        SVC_DELTAPACKETENTITIES = 41,
        SVC_CHOKE = 42,
        SVC_RESOURCELIST = 43,
        SVC_NEWMOVEVARS = 44,
        SVC_RESOURCEREQUEST = 45,
        SVC_CUSTOMIZATION = 46,
        SVC_CROSSHAIRANGLE = 47,
        SVC_SOUNDFADE = 48,
        SVC_FILETXFERFAILED = 49,
        SVC_HLTV = 50,
        SVC_DIRECTOR = 51,
        SVC_VOICEINIT = 52,
        SVC_VOICEDATA = 53,
        SVC_SENDEXTRAINFO = 54,
        SVC_TIMESCALE = 55,
        SVC_RESOURCELOCATION = 56,
        SVC_SENDCVARVALUE = 57,
        SVC_SENDCVARVALUE2 = 58,

        SVC_NUM_VALUES
    };

    typedef uint8_t byte;

    using std::string;


    struct simple_msg_t
    {
    };

    template <typename T>
    struct is_simple_msg
        : std::integral_constant<bool, std::is_base_of_v<simple_msg_t, T>>
    {
    };

    template <typename T>
    constexpr bool is_simple_msg_v = is_simple_msg<T>::value;


    struct complex_msg_t
    {
    };

    template <typename T>
    struct is_complex_msg
        : std::integral_constant<bool, std::is_base_of_v<complex_msg_t, T>>
    {
    };

    template <typename T>
    constexpr bool is_complex_msg_v = is_complex_msg<T>::value;


    template <msg_type_e>
    struct msg_t
        : complex_msg_t
    {
    };

    typedef short coord;


#define DEF_MSG(id) template<> struct msg_t<id> : simple_msg_t
#define DEF_COMPLEX_MSG(id) template<> struct msg_t<id> : complex_msg_t
#define DEF_COMPLEX_MSG_BASE(id, base) template<> struct msg_t<id> : complex_msg_t, base

    DEF_MSG(SVC_BAD)
    {
        REFL_INNER(msg_t)
        REFL_END()
    };

    DEF_MSG(SVC_ADDANGLE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(AngleToAdd)
        REFL_END()

        short AngleToAdd;
    };

    DEF_MSG(SVC_CDTRACK)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Track )
            REFL_ENTRY(LoopTrack)
        REFL_END()

        byte Track;
        byte LoopTrack;
    };

    DEF_MSG(SVC_CENTERPRINT)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Message)
        REFL_END()

        string Message;
    };

    DEF_MSG(SVC_CHOKE)
    {
        REFL_INNER(msg_t)
        REFL_END()
    };

    DEF_MSG(SVC_CROSSHAIRANGLE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(PitchAngleX5)
            REFL_ENTRY(YawAngleX5 )
        REFL_END()

        char PitchAngleX5;
        char YawAngleX5;
    };

    DEF_MSG(SVC_CUSTOMIZATION)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(PlayerIndex )
            REFL_ENTRY(Type )
            REFL_ENTRY(Name )
            REFL_ENTRY(Index )
            REFL_ENTRY(DownloadSize)
            REFL_ENTRY(Flags )
            REFL_ENTRY(MD5Hash )
        REFL_END()

        byte PlayerIndex;
        byte Type;
        string Name;
        short Index;
        long DownloadSize;
        byte Flags;
        std::array<byte, 16> MD5Hash;
    };

    DEF_MSG(SVC_CUTSCENE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Text)
        REFL_END()

        string Text;
    };

    DEF_MSG(SVC_DAMAGE)
    {
        REFL_INNER(msg_t)
        REFL_END()
    };

    DEF_MSG(SVC_DECALNAME)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(PositionIndex)
            REFL_ENTRY(DecalName )
        REFL_END()

        byte PositionIndex;
        string DecalName;
    };

    DEF_MSG(SVC_DISCONNECT)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Reason)
        REFL_END()

        string Reason;
    };

    DEF_MSG(SVC_FILETXFERFAILED)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(FileName)
        REFL_END()

        string FileName;
    };

    DEF_MSG(SVC_FINALE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Text)
        REFL_END()

        string Text;
    };

    DEF_MSG(SVC_FOUNDSECRET)
    {
        REFL_INNER(msg_t)
        REFL_END()
    };

    DEF_MSG(SVC_HLTV)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Mode)
        REFL_END()

        enum mode_t : uint8_t
        {
            HLTV_ACTIVE,
            HLTV_STATUS,
            HLTV_LISTEN,
        };

        mode_t Mode;
    };

    DEF_MSG(SVC_INTERMISSION)
    {
        REFL_INNER(msg_t)
        REFL_END()
    };

    DEF_MSG(SVC_KILLEDMONSTER)
    {
        REFL_INNER(msg_t)
        REFL_END()
    };

    DEF_MSG(SVC_LIGHTSTYLE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(index )
            REFL_ENTRY(lightinfo)
        REFL_END()

        byte index;
        string lightinfo;
    };

    DEF_MSG(SVC_NEWMOVEVARS)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Gravity )
            REFL_ENTRY(StopSpeed )
            REFL_ENTRY(MaxSpeed )
            REFL_ENTRY(SpectatorMaxSpeed)
            REFL_ENTRY(Accelerate )
            REFL_ENTRY(AirAccelerate )
            REFL_ENTRY(WaterAccelerate )
            REFL_ENTRY(Friction )
            REFL_ENTRY(EdgeFriction )
            REFL_ENTRY(WaterFriction )
            REFL_ENTRY(EntGravity )
            REFL_ENTRY(Bounce )
            REFL_ENTRY(StepSize )
            REFL_ENTRY(MaxVelocity )
            REFL_ENTRY(ZMax )
            REFL_ENTRY(WaveHeigth )
            REFL_ENTRY(Footsteps )
            REFL_ENTRY(RollAngle )
            REFL_ENTRY(RollSpeed )
            REFL_ENTRY(SkyColorRed )
            REFL_ENTRY(SkyColorGreen )
            REFL_ENTRY(SkyColorBlue )
            REFL_ENTRY(SkyVecX )
            REFL_ENTRY(SkyVecY )
            REFL_ENTRY(SkyVecZ )
            REFL_ENTRY(SkyName )
        REFL_END()

        float Gravity;
        float StopSpeed;
        float MaxSpeed;
        float SpectatorMaxSpeed;
        float Accelerate;
        float AirAccelerate;
        float WaterAccelerate;
        float Friction;
        float EdgeFriction;
        float WaterFriction;
        float EntGravity;
        float Bounce;
        float StepSize;
        float MaxVelocity;
        float ZMax;
        float WaveHeigth;
        byte Footsteps;
        float RollAngle;
        float RollSpeed;
        float SkyColorRed;
        float SkyColorGreen;
        float SkyColorBlue;
        float SkyVecX;
        float SkyVecY;
        float SkyVecZ;
        string SkyName;
    };

    DEF_MSG(SVC_NEWUSERMSG)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Index)
            REFL_ENTRY(Size)
            REFL_ENTRY(Name)
        REFL_END()

        byte Index;
        int8_t Size;
        std::array<char, 16> Name;
    };

    DEF_MSG(SVC_NOP)
    {
        REFL_INNER(msg_t)
        REFL_END()
    };

    DEF_MSG(SVC_PARTICLE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(OriginX )
            REFL_ENTRY(OriginY )
            REFL_ENTRY(OriginZ )
            REFL_ENTRY(DirectionXX16 )
            REFL_ENTRY(DirectionYX16 )
            REFL_ENTRY(DirectionZX16 )
            REFL_ENTRY(Count )
            REFL_ENTRY(Color )
        REFL_END()

        coord OriginX;
        coord OriginY;
        coord OriginZ;
        char DirectionXX16;
        char DirectionYX16;
        char DirectionZX16;
        byte Count;
        byte Color;
    };

    DEF_MSG(SVC_PRINT)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Message)
        REFL_END()

        string Message;
    };

    DEF_MSG(SVC_RESOURCELOCATION)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(sv_downloadurl)
        REFL_END()

        string sv_downloadurl;
    };

    DEF_MSG(SVC_RESOURCEREQUEST)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(SpawnCount)
            REFL_ENTRY(Zero )
        REFL_END()

        long SpawnCount;
        long Zero;
    };

    DEF_MSG(SVC_ROOMTYPE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Value)
        REFL_END()

        //0 = Normal (off)
        //1 = Generic
        //2 = Metal Small
        //3 = Metal Medium
        //4 = Metal Large
        //5 = Tunnel Small
        //6 = Tunnel Medium
        //7 = Tunnel Large
        //8 = Chamber Small
        //9 = Chamber Medium
        //10 = Chamber Large
        //11 = Bright Small
        //12 = Bright Medium
        //13 = Bright Large
        //14 = Water 1
        //15 = Water 2
        //16 = Water 3
        //17 = Concrete Small
        //18 = Concrete Medium
        //19 = Concrete Large
        //20 = Big 1
        //21 = Big 2
        //22 = Big 3
        //23 = Cavern Small
        //24 = Cavern Medium
        //25 = Cavern Large
        //26 = Weirdo 1
        //27 = Weirdo 2
        //28 = Weirdo 3

        short Value;
    };

    DEF_MSG(SVC_SENDCVARVALUE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Name)
        REFL_END()

        string Name;
    };

    DEF_MSG(SVC_SENDCVARVALUE2)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(RequestID)
            REFL_ENTRY(Name )
        REFL_END()

        long RequestID;
        string Name;
    };

    DEF_MSG(SVC_SENDEXTRAINFO)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(FallbackDir)
            REFL_ENTRY(CanCheat )
        REFL_END()

        string FallbackDir;
        byte CanCheat;
    };

    DEF_MSG(SVC_SERVERINFO)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Protocol )
            REFL_ENTRY(SpawnCount )
            REFL_ENTRY(MapCRC )
            REFL_ENTRY(ClientDLLHash)
            REFL_ENTRY(MaxPlayers )
            REFL_ENTRY(PlayerIndex )
            REFL_ENTRY(IsDeathmatch )
            REFL_ENTRY(GameDir )
            REFL_ENTRY(Hostname )
            REFL_ENTRY(MapFileName )
            REFL_ENTRY(Mapcycle )
            REFL_ENTRY(Zero )
        REFL_END()

        long Protocol;
        long SpawnCount;
        long MapCRC;
        std::array<byte, 16> ClientDLLHash;
        byte MaxPlayers;
        byte PlayerIndex;
        byte IsDeathmatch;
        string GameDir;
        string Hostname;
        string MapFileName;
        string Mapcycle;
        byte Zero;
    };

    DEF_MSG(SVC_SETANGLE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(AnglePitch)
            REFL_ENTRY(AngleYaw )
            REFL_ENTRY(AngleRoll )
        REFL_END()

        short AnglePitch;
        short AngleYaw;
        short AngleRoll;
    };

    DEF_MSG(SVC_SETPAUSE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(IsPaused)
        REFL_END()

        byte IsPaused;
    };

    DEF_MSG(SVC_SETVIEW)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(EntityIndex)
        REFL_END()

        short EntityIndex;
    };

    DEF_MSG(SVC_SIGNONNUM)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(One)
        REFL_END()

        byte One;
    };

    DEF_MSG(SVC_SOUNDFADE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(InitialPercent)
            REFL_ENTRY(HoldTime )
            REFL_ENTRY(FadeOutTime )
            REFL_ENTRY(FadeInTime )
        REFL_END()

        byte InitialPercent;
        byte HoldTime;
        byte FadeOutTime;
        byte FadeInTime;
    };

    DEF_MSG(SVC_SPAWNSTATICSOUND)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(OriginX )
            REFL_ENTRY(OriginY )
            REFL_ENTRY(OriginZ )
            REFL_ENTRY(SoundIndex )
            REFL_ENTRY(VolumeX255 )
            REFL_ENTRY(AttenuationX64)
            REFL_ENTRY(EntityIndex )
            REFL_ENTRY(Flags )
            REFL_ENTRY(one_byte_is_missing )
        REFL_END()

        coord OriginX;
        coord OriginY;
        coord OriginZ;
        short SoundIndex;
        byte VolumeX255;
        byte AttenuationX64;
        short EntityIndex;
        byte Flags;
        byte one_byte_is_missing;
    };

    DEF_MSG(SVC_STOPSOUND)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(EntityIndex)
        REFL_END()

        short EntityIndex;
    };

    DEF_MSG(SVC_STUFFTEXT)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Command)
        REFL_END()

        string Command;
    };

    DEF_MSG(SVC_TIME)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(Time)
        REFL_END()

        float Time;
    };

    DEF_MSG(SVC_TIMESCALE)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(TimeScale)
        REFL_END()

        float TimeScale;
    };

    DEF_MSG(SVC_UPDATEUSERINFO)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(ClientIndex )
            REFL_ENTRY(ClientUserID )
            REFL_ENTRY(ClientUserInfo )
            REFL_ENTRY(ClientCDKeyHash)
        REFL_END()

        byte ClientIndex;
        long ClientUserID;
        string ClientUserInfo;
        std::array<byte, 16> ClientCDKeyHash;
    };

    DEF_MSG(SVC_VERSION)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(ProtocolVersion)
        REFL_END()

        long ProtocolVersion;
    };

    DEF_MSG(SVC_VOICEINIT)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(CodecName)
            REFL_ENTRY(Quality )
        REFL_END()

        string CodecName;
        byte Quality;
    };

    DEF_MSG(SVC_WEAPONANIM)
    {
        REFL_INNER(msg_t)
            REFL_ENTRY(SequenceNumber )
            REFL_ENTRY(WeaponmodelBodygroup)
        REFL_END()

        byte SequenceNumber;
        byte WeaponmodelBodygroup;
    };

    DEF_COMPLEX_MSG(SVC_DELTADESCRIPTION)
    {
        string Name;
		vector<delta_desc_entry_t> Entries;
    };

    DEF_COMPLEX_MSG(SVC_RESOURCELIST)
    {
        struct resource_t
        {
            uint32_t type;
            string name;
            uint32_t index, size, flags;
            bool has_extra_info;
        };

        vector<resource_t> Resources;
    };

    struct msg_spawnstatic_base_t
    {
        typedef short angle;

        std::array<uint8_t, 18> unknown_data;
        byte 	RenderMode ;        

        REFL_INNER(msg_spawnstatic_base_t)
            REFL_ENTRY(unknown_data)        
            REFL_ENTRY(RenderMode)        
        REFL_END()
    };

    DEF_COMPLEX_MSG_BASE(SVC_SPAWNSTATIC, msg_spawnstatic_base_t)
    {
        struct render_params_t
        {
            byte 	RenderAmt   ;
            byte 	RenderColorR;
            byte 	RenderColorG;
            byte 	RenderColorB;
            byte 	RenderFX    ;         

            REFL_INNER(render_params_t)
                REFL_ENTRY(RenderAmt   )
                REFL_ENTRY(RenderColorR)
                REFL_ENTRY(RenderColorG)
                REFL_ENTRY(RenderColorB)
                REFL_ENTRY(RenderFX    )         
            REFL_END()
        };

        optional<render_params_t> RenderParams;
    };

    DEF_COMPLEX_MSG(SVC_CLIENTDATA)
    {
        delta_struct_t client_data;
        vector<delta_struct_t> weapon_data;
    };

    DEF_COMPLEX_MSG(SVC_SPAWNBASELINE)
    {
        struct ent_t
        {
            uint32_t index = 0;
            bool custom = false;
            delta_struct_t baseline;
        };

        vector<ent_t> ents;
        vector<delta_struct_t> instanced_baselines;
    };

    DEF_COMPLEX_MSG(SVC_PACKETENTITIES)
    {
        struct ent_t
        {
            uint32_t index = 0;
            bool custom = false;
            optional<uint32_t> baseline_index;
            delta_struct_t delta;
        };

        vector<ent_t> ents;
    };
    DEF_COMPLEX_MSG(SVC_DELTAPACKETENTITIES)
    {
        struct ent_t
        {
            uint32_t index = 0;
            bool custom = false;
            delta_struct_t delta;
        };

        uint8_t source_frame;
        vector<ent_t> ents;
        vector<uint32_t> to_remove;
    };
#undef DEF_MSG
} // namespace hl_netmsg
