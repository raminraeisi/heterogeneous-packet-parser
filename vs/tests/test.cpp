#include <gtest/gtest.h>
#include <istream>
#include <fstream>
#include <filesystem>
#include <Packet/StreamPacketParser.hpp>
#include <Windows.h>
TEST(InputStreams, SimpleBatteryStream) {
    std::string bin = { '\x01', '\x00', '\x00', '\x03', '\xe8', '\x03', '\xef' };
    std::istringstream input(bin);
    std::ostringstream output;
    cdp::packet::StreamPacketParser parser(input, output);
    parser.parse();
    std::string str(output.str());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\x00'), str.end());
    EXPECT_EQ(str, "B;1;HIGH");
}

TEST(InputStreams, CodingTestBinFile) {
    std::string str("");
    char exe[MAX_PATH];
    GetModuleFileNameA(NULL, exe, MAX_PATH);
    std::string fileName(exe);
    fileName = std::string(exe).substr(0, fileName.find_last_of("\\/"));
    fileName.append("\\CodingTest.bin");
    if (std::filesystem::exists(fileName))
    {
        std::ifstream file(fileName, std::ios::binary | std::ios::ate);
        std::ostringstream output;
        cdp::packet::StreamPacketParser parser(file, output);
        parser.parse();
        str = output.str();
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        str.erase(std::remove(str.begin(), str.end(), '\x00'), str.end());
    }
    EXPECT_EQ(str, "B;1;HIGHS;3;0-1S;7;1-2B;7;MEDS;10;2-3S;17;3-2S;21;2-0");
}