#pragma once

namespace hl_netmsg
{
    
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

    SVC_NUM_VALUES
};

typedef uint8_t byte;

using std::string;

template<msg_type_e>
struct msg_t
{
    constexpr static bool ok = false;
};

struct ok_msg_t
{
    constexpr static bool ok = true;
};


struct coord {};

#define DEF_MSG(id) template<> struct msg_t<id> : ok_msg_t

DEF_MSG(SVC_BAD)
{
};
    
DEF_MSG(SVC_ADDANGLE)
{
    short 	AngleToAdd;
};

DEF_MSG(SVC_CDTRACK)
{
    byte 	Track   ;
    byte 	LoopTrack;
};

DEF_MSG(SVC_CENTERPRINT)
{
    string 	Message;
};

DEF_MSG(SVC_CHOKE)
{
};

DEF_MSG(SVC_CROSSHAIRANGLE)
{
    char 	PitchAngleX5;
    char 	YawAngleX5; 
};

DEF_MSG(SVC_CUSTOMIZATION)
{
    byte 	PlayerIndex ;
    byte 	Type        ;
    string 	Name        ;
    short 	Index       ;
    long 	DownloadSize;
    byte 	Flags       ;
    byte 	MD5Hash[16] ;
};

DEF_MSG(SVC_CUTSCENE)
{
    string 	Text;
};

DEF_MSG(SVC_DAMAGE)
{
};

DEF_MSG(SVC_DECALNAME)
{
    byte 	PositionIndex;
    string 	DecalName    ;
};

DEF_MSG(SVC_DISCONNECT)
{
    string 	Reason;
};

DEF_MSG(SVC_FILETXFERFAILED)
{
    string 	FileName;
};

DEF_MSG(SVC_FINALE)
{
    string 	Text;
};

DEF_MSG(SVC_FOUNDSECRET)
{
};

DEF_MSG(SVC_HLTV)
{
    enum mode_t : uint8_t
    {
        HLTV_ACTIVE,
        HLTV_STATUS	,
        HLTV_LISTEN	,
    };

    mode_t Mode;
};

DEF_MSG(SVC_INTERMISSION)
{
};

DEF_MSG(SVC_KILLEDMONSTER)
{
};

DEF_MSG(SVC_LIGHTSTYLE)
{
    byte 	index    ;
    string 	lightinfo; 
};

DEF_MSG(SVC_NEWMOVEVARS)
{
    float 	Gravity          ;
    float 	StopSpeed        ;
    float 	MaxSpeed         ;
    float 	SpectatorMaxSpeed;
    float 	Accelerate       ;
    float 	AirAccelerate    ;
    float 	WaterAccelerate  ;
    float 	Friction         ;
    float 	EdgeFriction     ;
    float 	WaterFriction    ;
    float 	EntGravity       ;
    float 	Bounce           ;
    float 	StepSize         ;
    float 	MaxVelocity      ;
    float 	ZMax             ;
    float 	WaveHeigth       ;
    byte 	Footsteps        ;
    float 	RollAngle        ;
    float 	RollSpeed        ;
    float 	SkyColorRed      ;
    float 	SkyColorGreen    ;
    float 	SkyColorBlue     ;
    float 	SkyVecX          ;
    float 	SkyVecY          ;
    float 	SkyVecZ          ;
    string 	SkyName          ;
};

//DEF_MSG(SVC_NEWUSERMSG)
//{
//    byte 	Index;
//    byte 	Size;
//    uint16_t 	Name; ???
//};

DEF_MSG(SVC_NOP)
{
};

DEF_MSG(SVC_PARTICLE)
{
    coord 	OriginX        ;
    coord 	OriginY        ;
    coord 	OriginZ        ;
    char 	DirectionXX16;
    char 	DirectionYX16;
    char 	DirectionZX16;
    byte 	Count          ;
    byte 	Color          ;
};

DEF_MSG(SVC_PRINT)
{
    string 	Message;
};

DEF_MSG(SVC_RESOURCELOCATION)
{
    string 	sv_downloadurl;
};

DEF_MSG(SVC_RESOURCEREQUEST)
{
    long 	SpawnCount;
    long 	Zero      ;
};

DEF_MSG(SVC_ROOMTYPE)
{
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

    short 	Value;
};

DEF_MSG(SVC_SENDCVARVALUE)
{
    string 	Name;
};

DEF_MSG(SVC_SENDCVARVALUE2)
{
    long 	RequestID;
    string 	Name     ;
};

DEF_MSG(SVC_SENDEXTRAINFO)
{
    string 	FallbackDir;
    byte 	CanCheat   ;
};

DEF_MSG(SVC_SERVERINFO)
{
    long 	Protocol         ;
    long 	SpawnCount       ;
    long 	MapCRC           ;
    byte 	ClientDLLHash[16];
    byte 	MaxPlayers       ;
    byte 	PlayerIndex      ;
    byte 	IsDeathmatch     ;
    string 	GameDir          ;
    string 	Hostname         ;
    string 	MapFileName      ;
    string 	Mapcycle         ;
    byte 	Zero             ;
};

DEF_MSG(SVC_SETANGLE)
{
    short 	AnglePitch;
    short 	AngleYaw  ;
    short 	AngleRoll ;
};

DEF_MSG(SVC_SETPAUSE)
{
    byte 	IsPaused;
};

DEF_MSG(SVC_SETVIEW)
{
    short 	EntityIndex;
};

DEF_MSG(SVC_SIGNONNUM)
{
    byte One;
};

DEF_MSG(SVC_SOUNDFADE)
{
    byte 	InitialPercent;
    byte 	HoldTime      ;
    byte 	FadeOutTime   ;
    byte 	FadeInTime    ;
};

DEF_MSG(SVC_SPAWNSTATICSOUND)
{
    coord 	OriginX       ;
    coord 	OriginY       ;
    coord 	OriginZ       ;
    short 	SoundIndex    ;
    byte 	VolumeX255    ;
    byte 	AttenuationX64;
    short 	EntityIndex   ;
    byte 	Flags         ;
};

DEF_MSG(SVC_STOPSOUND)
{
    short 	EntityIndex;
};

DEF_MSG(SVC_STUFFTEXT)
{
    string 	Command;
};

DEF_MSG(SVC_TIME)
{
    float 	Time;
};

DEF_MSG(SVC_TIMESCALE)
{
    float 	TimeScale;
};

DEF_MSG(SVC_UPDATEUSERINFO)
{
    byte 	ClientIndex        ;
    long 	ClientUserID       ;
    string 	ClientUserInfo     ;
    byte 	ClientCDKeyHash[16];
};

DEF_MSG(SVC_VERSION)
{
    long 	ProtocolVersion;
};

DEF_MSG(SVC_VOICEINIT)
{
    string 	CodecName;
    byte 	Quality  ;
};

DEF_MSG(SVC_WEAPONANIM)
{
    byte 	SequenceNumber      ;
    byte 	WeaponmodelBodygroup;
};

#undef DEF_MSG

} // namespace hl_netmsg