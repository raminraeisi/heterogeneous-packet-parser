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
#include <Packet/StreamPacketParser.hpp>

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		std::string fileName(argv[1]);
		if (std::filesystem::exists(fileName))
		{
			std::ifstream file(fileName, std::ios::binary);
			cdp::packet::StreamPacketParser packetParser(file, std::cout);
			packetParser.parse();
		}
		else {
			std::cout << "Input file not found" << std::endl;
		}
	}
	else {
		std::cout << "No input file is provided" << std::endl;
	}
	return(0);
}
