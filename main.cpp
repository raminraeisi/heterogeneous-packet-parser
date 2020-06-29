/*! \mainpage CDP Assessment Project
 *
 * \section intro_sec Introduction
 *
 * This is my proposed solution to the defined problem described
 * in the provided doumentation.
 *
 * \section install_sec Building
 * You can find the build.bat file in the root of the solution 
 * which you can run to build the project built in release mode
 *
 */
#include <fstream>
#include <filesystem>
#include <Packet/PacketFactory.hpp>

#define PACKET_VOLTAGE_CURRENT 0
#define PACKET_BATTERY         1

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
  if (argc == 2)
  {
      std::string fileName(argv[1]);
      if (std::filesystem::exists(fileName))
      {
          std::ifstream file(fileName, std::ios::binary | std::ios::ate);
          if (file.tellg()) {
              file.seekg(0, std::ios::beg);

              uint8_t packetType;
              auto& packetFactory = cdp::factory::PacketFactory::getInstance();

              /// Initializes output stream of the voltage/current packet
              {
                  auto packet = packetFactory.GetObject(PACKET_VOLTAGE_CURRENT);
                  if (packet) {
                      packet->setOutputStream(&std::cout);
                  }

              }

              /// Initializes output stream of the battery packet
              {
                  auto packet = packetFactory.GetObject(PACKET_BATTERY);
                  if (packet) {
                      packet->setOutputStream(&std::cout);
                  }

              }

              /// Iterating over the file and parse packets
              file >> packetType;
              while (!file.eof()) {
				  /// Retrieves the corresponding packet instance from the registry
                  auto packet = packetFactory.GetObject(packetType);

                  if (packet) {
                      std::vector<char> buffer(packet->length());

                      buffer[0] = packetType;
                      file.read(&buffer[1], packet->length() - 1);

                      packet->updateFromMemory(&buffer[0]);

                      file >> packetType;
                  }
              }
              file.close();
          }
      }
      else {
          std::cout << "Input file not found" << std::endl;
      }
  }
  else {
      std::cout << "No input file is provided" << std::endl;
  }
  return 0;
}
