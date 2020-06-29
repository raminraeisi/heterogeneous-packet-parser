/**
 * @file include/Packet/Packet.hpp
 *
 * @author Ramin Raeisi <rr@epen.io>
 */

#ifndef PACKET_HPP
#define PACKET_HPP

#include <Globals.hpp>
#include <Packet/PacketBase.hpp>
#include <Packet/PacketFactory.hpp>
#include <Factory/FactoryRegistrar.hpp>

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
template<class NewPacket, cdp::globals::PacketIDType ID, class _First, class ... _Rest>
class PacketTemplate : public cdp::packet::PacketBase, 
    protected cdp::factory::FactoryRegistrar<cdp::packet::PacketBase, NewPacket, cdp::factory::PacketFactory, ID>
{
public:
    /// Default constructor which initializes PacketBase class values
    /// and internal strucutre of t
    PacketTemplate() : _prevPacketDeltaTime(0) {
        _packetData = _data;
        _packetLength = PacketTemplate::_tupleLength;
        setElementPointers();
    }

public:
    /// Updates the internal packet data from an external memry region
    virtual bool updateFromMemory(void* fromAddress) override {
        if (!PacketBase::updateFromMemory(fromAddress)) {
            return(false);
        }

        /// Will check the target endianness and correct the byte order of fields
        /// in RAM if needed
        if (_endianness != cdp::globals::Endianness::BigEndian) {
            std::apply(
                [&](auto&&... elem) {
                    ((*elem = cdp::globals::swapEndian(*elem)), ...);
                }
            , _tuple);
        }

        auto& time = *std::get<1>(_tuple);

        auto dt = time - _prevPacketDeltaTime;
        CHECK_AND_REPORT_ERROR_IF(dt, < , 0, "Time value error");

        _prevPacketDeltaTime = dt;
        return(true);
    }

protected:
    /// Automatically calculates the packet length by sum of the tuple element sizes
    static constexpr size_t _tupleLength = cdp::globals::sizeOfTupleElements(std::tuple<_First, _Rest...>{});
    
    /// Internal representation of the packet data
    uint8_t _data[PacketTemplate::_tupleLength];
    
    /// The tuple which stores the pointers to the fields in the packet buffer
    std::tuple<_First, _Rest...> _tuple{};

    /// Holds the delta time between voltage/current packets
    uint32_t _prevPacketDeltaTime;
private:
    /// Updates the typed pointers in the tuple to point to the corespondent fields
    /// using the correspondent type sizes
    void setElementPointers() {
        size_t offsets[std::tuple_size<decltype(_tuple)>::value + 1]{0};
        size_t index = 1;
        std::apply(
            [&](auto&&... elem) {
                ((offsets[index++] = (sizeof(*elem) + offsets[(--index)++])), ...);
            }, _tuple);
        std::apply(
            [&](auto&&... elem) {
                size_t index = 0;
                ((elem = reinterpret_cast<typename std::remove_reference<decltype(elem)>::type>(&_data[0] + offsets[index++])), ...);
            }
        , _tuple);
    }
};

    } // namespace packet
} // namespace cdp

#endif // !PACKET_HPP
