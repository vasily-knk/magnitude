#include "msg_reader.h"

namespace hl_netmsg
{
    
void msg_reader::operator()(string &value, char const *)
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