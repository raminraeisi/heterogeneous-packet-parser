/**
 * @file include/Packet/PacketInterface.hpp
 *
 * @author Ramin Raeisi <rr@epen.io>
 */
#ifndef PACKETBASE_HPP
#define PACKETBASE_HPP

#include <string_view>
#include <Globals.hpp>

namespace cdp {
    namespace packet {

        /**
        * @brief: Base class of all packets, every new packet type should be derived from this
        * class as the base
        */
        class PacketBase {
        public:
            /// Constructor
            PacketBase() : _outputStream(nullptr), _packetLength(0), _packetData(nullptr) {
            }

            /// Reports an error message to the specified output stream
            void reportErrorLine(std::string_view str) {
                if (_outputStream)
                    (*_outputStream) << "ERR:<" << str << ">" << std::endl;
            }

            /// Reports a general message to the specified output stream
            void reportMessageLine(std::string_view str) {
                if (_outputStream)
                    (*_outputStream) << str << std::endl;
            }

            /// Set the output stream of the class which will be used 
            /// for the output reporting
            void setOutputStream(std::ostream* stream) {
                _outputStream = stream;
            }

            /// Returns the length of the packet
            size_t length() {
                return(_packetLength);
            }

            /// Updates the internal packet data using a provided source memory 
            /// region and returns the result if succeeded or not based on the 
            /// calculated error and the real error field
            virtual bool updateFromMemory(uint8_t* fromAddress) {
                size_t currentFieldIndex;
                size_t packetIndex = 0;
                uint8_t error = 0;
                int8_t delta = +1;

                if (_endianness != cdp::globals::Endianness::BigEndian)
                    delta = -1;

                for (size_t i = 0; i < _fieldOffsets.size()-1; ++i) {

                    size_t currentFieldLength = _fieldOffsets[i + 1] - _fieldOffsets[i];
                    if (_endianness == cdp::globals::Endianness::LittleEndian) {
                        currentFieldIndex = _fieldOffsets[i + 1] - 1;
                    }
                    else {
                        currentFieldIndex = _fieldOffsets[i];
                    }
                    while (currentFieldLength--) {
                        _packetData[packetIndex++] = fromAddress[currentFieldIndex];
                        if (packetIndex < _packetLength)
                            error += fromAddress[currentFieldIndex];
                        currentFieldIndex += delta;
                    }
                }
                return(error == _packetData[_packetLength - 1]);
            }
        protected:
            /// Holds a pointer to the packet data
            uint8_t* _packetData;
            /// Holds the packet length
            size_t _packetLength;

            std::vector<size_t> _fieldOffsets;
            /// Holds the memory endianness of the target on which the packet exists
            inline static cdp::globals::Endianness _endianness = cdp::globals::getEndianness();
            /// Holds a pointer to the desired output stream
            std::ostream* _outputStream;

        private:
        };
    } // namespace packet
} // namespace cdp
#endif // !PACKETBASE_HPP
