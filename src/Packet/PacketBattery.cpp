#ifndef PACKETBATTERY_HPP
#define PACKETBATTERY_HPP

#include <Packet/PacketTemplate.hpp>

namespace cdp {
    namespace packet {

/**
* @brief: A template based class which is derived from PacketBase to add the
* following capabilities
* 1- Forming the internal packet structure using std::tuple
* 2- Auto registration of the derived packet classes in the PacketFactory 
* 3- Auto correcting the endianness 
* Actually we have an internal buffer holding whole data of the packet and the 
* tuple will be holding the typed pointers to the fileds in the buffer

* @tparam NewPacket refers to the new packet class to be defined
* @tparam ID desired new packet ID
* @tparam _First Pointer type to the first field of the packet
* @tparam _Rest... Pointer type to the rest fields of the packet
*/
class PacketBattery : public cdp::packet::PacketTemplate<PacketBattery, 1, uint8_t*, uint32_t*, uint8_t*, uint8_t*> {
    using PacketTemplate::PacketTemplate;
public:
    /// Constructor
    PacketBattery() {
        /// Prevents the compiler from optimising out the _registered variable in the 
        /// FactoryRegistrar class 
        (void)_registered;
    }

    /// Overrides the base class method to specialize its operation based on the 
    /// battery packet specs, reports to the output as well
    bool updateFromMemory(void* fromAddress) final {
        if (!PacketTemplate::updateFromMemory(fromAddress)) {
            return(false);
        }

        /// defines structure binding for a clear view of tuple
        auto& [packetType, time, batteryState, check] = _tuple;
            
        CHECK_AND_REPORT_ERROR_IF(*batteryState, > , 3, "Wrong battery state value");
            
        const char *states[] = { "VLOW", "LOW", "MED", "HIGH" };
        reportMessageLine(cdp::globals::stringFormat("B;%d;%s", *time / 1000, states[*batteryState]));

        return(true);
    }

};

    } // namespace packet
} // namespace cdp
#endif

