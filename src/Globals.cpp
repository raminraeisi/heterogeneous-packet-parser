#include <Globals.hpp>

namespace cdp
{
    namespace globals {
        Endianness getEndianness()
        {
            unsigned int i = 1;
            char* c = (char*)&i;
            if (*c)
                return(cdp::globals::Endianness::LittleEndian);
            else
                return(cdp::globals::Endianness::BigEndian);
        }
    } // namespace general
} // namespace cdp