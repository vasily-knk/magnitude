#include "msg_reader.h"

namespace hl_netmsg
{
void msg_reader::read_msg(msg_t<SVC_DELTADESCRIPTION>& msg)
{
    read_field(msg.Name);
    read_field(msg.NumFields);

    int aaa = 5;
}

void msg_reader::read_field(string &value)
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