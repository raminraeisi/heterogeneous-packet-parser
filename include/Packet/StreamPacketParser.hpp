#ifndef STREAMPACKETPARSER_HPP
#define STREAMPACKETPARSER_HPP

#include <istream>
#include <Packet/PacketFactory.hpp>

namespace cdp {
	namespace packet {

		/**
		* @brief: It is a class which will be parsing a stream of packets. It gets an argument of
		* type std::istream and will walk through the stream and look for the packets to parse
		*/
		class StreamPacketParser {
		public:
			/// Constructor
			StreamPacketParser(std::istream& inputStream, std::ostream& outputStream) 
				: _istream(inputStream), _ostream(outputStream) {
			}

			/// Will be called to parse the stream
			void parse() {
				_istream.seekg(0, std::ios::beg);

				uint8_t packetType;
				auto& packetFactory = cdp::factory::PacketFactory::getInstance();

				/// Iterating over the file and parse packets
				try {
					while (!_istream.eof()) {
						_istream >> packetType;
						/// Retrieves the corresponding packet instance from the registry
						auto packet = packetFactory.GetObject(packetType);

						if (packet) {
							packet->setOutputStream(&_ostream);
							std::vector<char> buffer(packet->length());

							buffer[0] = packetType;

							_istream.read(&buffer[1], packet->length() - 1);
							packet->updateFromMemory(reinterpret_cast<uint8_t*>(&buffer[0]));
						}
					}
				}
				catch (std::istream::failure e) {
					std::cerr << "exception reading the input file";
				}
			}
		private:
			/// Will be holding a reference to the desired stream object to be used as input of parsing
			std::istream& _istream;
			std::ostream& _ostream;
		};

	} // namespace packet
} // namespace cdp

#endif

