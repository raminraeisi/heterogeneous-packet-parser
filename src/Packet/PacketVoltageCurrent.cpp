#ifndef PACKETVOLTAGECURRENT_HPP
#define PACKETVOLTAGECURRENT_HPP

#include <Packet/PacketTemplate.hpp>
#include <Packet/PacketFactory.hpp>
#include <Factory/FactoryRegistrar.hpp>

namespace cdp {
    namespace packet {
/**
* @brief: Represents the voltage/current packet defined in the provided document, ID = 0
*/
class PacketVoltageCurrent : public cdp::packet::PacketTemplate<PacketVoltageCurrent, 0, uint8_t*, uint32_t*, uint32_t*, uint64_t*, uint8_t*> {
public:
    /// Constructor
    PacketVoltageCurrent() : _currentState(0), _currentStateFixityTime(0) {
        /// Prevents the compiler from optimising out the _registered variable in the 
        /// FactoryRegistrar class 
        (void)_registered;
    }
public:
    /// Overrides the base class method to specialize its operation based on the 
    /// voltage/current packet requirements
    bool updateFromMemory(uint8_t* fromAddress) final {
        if (!PacketTemplate::updateFromMemory(fromAddress)) {
            return(false);
        }

        /// Defines structure binding for tuple elements
        auto& [packetType, time, voltage, current, check] = _tuple;
        auto tempNewState = cdp::globals::findRangeHelper(viablePowerRanges, std::multiplies{}, voltage, current);
        if (tempNewState < viablePowerRanges.size()) {
            tryToChangeState(tempNewState);
        }

        return(true);
    }

private:
    /// Performs the state changing and reports to output stream
    void changeState(size_t newState) {
        /// defines structure binding for a clear view of tuple elements
        auto& [packetType, time, voltage, current, check] = _tuple;
        reportMessageLine(cdp::globals::stringFormat("S;%d;%d-%d", *time / 1000, _currentState , newState));

        _currentState = newState;
        _currentStateFixityTime = 0;
    }
    
    /// Checks if it is feasible to change state, otherwise updates some internal values
    void tryToChangeState(size_t state) {
        if (state == _currentState) {
            _currentStateFixityTime += _prevPacketDeltaTime;
        }
        else {
            if (allowedStateTransitions.find(std::make_pair(_currentState, state)) != allowedStateTransitions.end() && 
                _currentStateFixityTime > SETTLING_TIME) {
                changeState(state);
            }
        }
    }

private:
    /// Holds the viable power state ranges
    inline static auto viablePowerRanges = std::vector<std::pair<size_t, size_t>>{ {0, 200}, { 300, 450 }, { 550, 650 }, { 800, 1200 } };
    
    /// Holds the allowed state transitions
    inline static auto allowedStateTransitions = std::unordered_map<std::pair<size_t, size_t>, bool, cdp::globals::hash_pair>{ 
        {{0, 1}, true}, {{1, 2}, true}, {{2, 3}, true}, {{3, 2}, true}, {{2, 0}, true} };

    /// Settling time of a new state
    static constexpr size_t SETTLING_TIME = 10;

    /// Hols the current power state 
    size_t _currentState;
    /// Hols the current power state fixity
    size_t _currentStateFixityTime;
};

    } // namespace packet
} // namespace cdp

#endif

