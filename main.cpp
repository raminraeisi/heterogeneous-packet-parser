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
#include <iostream>
#include <fstream>
#include <filesystem>
#include <Packet/PacketFactory.hpp>

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

				/// Iterating over the file and parse packets
				try {
					file >> packetType;
					while (!file.eof()) {
						/// Retrieves the corresponding packet instance from the registry
						auto packet = packetFactory.GetObject(packetType);

						if (packet) {
							packet->setOutputStream(&std::cout);
							std::vector<char> buffer(packet->length());

							buffer[0] = packetType;

							file.read(&buffer[1], packet->length() - 1);
							packet->updateFromMemory(reinterpret_cast<uint8_t*>(&buffer[0]));
							file >> packetType;

						}
					}
				}
				catch (std::ifstream::failure e) {
					std::cerr << "exception reading the input file";
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
