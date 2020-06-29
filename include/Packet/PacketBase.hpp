/**
 * @file include/Packet/PacketInterface.hpp
 *
 * @author Ramin Raeisi <rr@epen.io>
 */
#ifndef PACKETBASE_HPP
#define PACKETBASE_HPP

#include <cstring>
#include <cstdint>
#include <iostream>
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
            PacketBase() : _outputStream(nullptr) {
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

            /// Updates the internal packet data using from a provided memory region
            /// anbd returns the result if succeeded or not
            virtual bool updateFromMemory(void* fromAddress) {
                std::memcpy(_packetData, fromAddress, _packetLength);
                return(!hasError());
            }
        protected:
            /// Checks correctness of the Error Check Byte
            bool hasError() {
                bool ret;
                uint8_t error = 0, i;
                for (i = 0; i < _packetLength - 1; ++i) {
                    error += _packetData[i];
                }
                if (ret = _packetData[i] != error; ret) {
                    reportErrorLine("ERR:Error Check");
                }
                return(ret);
            }

        protected:
            /// Holds a pointer to the packet data
            uint8_t* _packetData;
            /// Holds the packet length
            size_t _packetLength;
            /// Holds the memory endianness of the target on which the packet exists
            inline static cdp::globals::Endianness _endianness = cdp::globals::getEndianness();
            /// Holds a pointer to the desired output stream
            std::ostream* _outputStream;

        private:
        };
    } // namespace packet
} // namespace cdp
#endif // !PACKETBASE_HPP
