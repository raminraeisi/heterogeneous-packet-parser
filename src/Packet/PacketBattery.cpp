#ifndef PACKETBATTERY_HPP
#define PACKETBATTERY_HPP

#include <Packet/PacketTemplate.hpp>

namespace cdp {
	namespace packet {

		/**
		* @brief: Represents the battery packet defined in the provided document, ID = 1
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
			bool updateFromMemory(uint8_t* fromAddress) final {
				if (!PacketTemplate::updateFromMemory(fromAddress)) {
					return(false);
				}

				/// defines structure binding for a clear view of tuple
				auto& [packetType, time, batteryState, check] = _tuple;

				CHECK_AND_REPORT_ERROR_IF(*batteryState, > , 3, "Wrong battery state value");

				const char* states[] = { "VLOW", "LOW", "MED", "HIGH" };
				reportMessageLine(cdp::globals::stringFormat("B;%d;%s", *time / 1000, states[*batteryState]));

				return(true);
			}

		};

	} // namespace packet
} // namespace cdp
#endif

