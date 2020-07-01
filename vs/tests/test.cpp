#include <gtest/gtest.h>
#include <istream>
#include <fstream>
#include <filesystem>
#include <Packet/StreamPacketParser.hpp>
#include <Windows.h>
#include <string>
#include <vector>
#include <numeric>
#include <ctime>
#include "MemStream.hpp"

/// Inverts the endianness of a numeric variable of type T
template <typename T>
T swapEndian(T u)
{
    static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

std::vector<char> createValidBatteryPacket(uint32_t time, uint8_t state) {
    std::vector<char> buffer(7);
    OMemStream ms(&buffer[0], 7);

    ms << uint8_t(0x01);
    time = swapEndian(time);
    ms.write(reinterpret_cast<char*>(&time), 4);
    ms << state;

    *(buffer.end() - 1) = std::accumulate(buffer.begin(), buffer.end() - 1, 0);

    return(std::move(buffer));
}

std::vector<char> createValidVoltageCurrentPacket(uint32_t time, uint32_t voltage, uint64_t current) {
    std::vector<char> buffer(18);
    OMemStream ms(&buffer[0], 18);

    ms << uint8_t(0x00);
    time = swapEndian(time);
    ms.write(reinterpret_cast<char*>(&time), 4);
    voltage = swapEndian(voltage);
    ms.write(reinterpret_cast<char*>(&voltage), 4);
    current = swapEndian(current);
    ms.write(reinterpret_cast<char*>(&current), 8);

    *(buffer.end() - 1) = std::accumulate(buffer.begin(), buffer.end() - 1, 0);

    return(std::move(buffer));
}

TEST(InputStreams, IrrelevantStream) {
    std::string bin("FEIFHX\x01N38Xf\x02ejcnweoiv\x01nfqwo\x03iefcnqowcenfoi  \x00  qxne nxfkan doew\x01ihf qoiwehg qoiwre \
                    fckweqn fnqwex fiqwrsdgskdjgksjdksgjdkjgefq");
    std::istringstream input(bin);
    std::ostringstream output;
    cdp::packet::StreamPacketParser parser(input, output);
    parser.parse();
    std::string str(output.str());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    EXPECT_EQ(str, "");
}

TEST(InputStreams, IrrelevantStreamButOneCorrectBatteryPacket) {
    std::string bin("FEIFHX\x01N38Xf\x02ejcnweoiv\x01nfqwo\x03iefcnqowcenfoi  \x00  qxne nxfkan doew\x01ihf qoiwehg qoiwre \
                    fckweqn fnqwex fiqwrsdgskdjgksjdksgjdkjgefq");
    bin.append(std::string({ '\x01', '\x00', '\x00', '\x03', '\xe8', '\x03', '\xef' }));
    bin.append("nfdaisdnfiasdf\x00vsdasdfsdf\x01   jdkcJA Djkas fK FKh adsA DSKH dsfh D");
    std::istringstream input(bin);
    std::ostringstream output;
    cdp::packet::StreamPacketParser parser(input, output);
    parser.parse();
    std::string str(output.str());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    EXPECT_EQ(str, "B;1;HIGH");
}

TEST(InputStreams, SingleBatteryPacket) {
    std::string str("");

    std::vector<char> buffer{};
    auto& vec = createValidBatteryPacket(1000, 1);
    buffer.insert(buffer.end(), vec.begin(), vec.end());

    IMemStream ms(buffer.data(), buffer.size());
    std::ostringstream output;
    cdp::packet::StreamPacketParser parser(ms, output);
    parser.parse();

    str = output.str();
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    EXPECT_EQ(str, "B;1;LOW");
}

TEST(InputStreams, SingleBatteryPacketCrcError) {
    std::string str("");

    std::vector<char> buffer{};
    auto& vec = createValidBatteryPacket(1000, 1);
    vec[vec.size() - 1]--;
    buffer.insert(buffer.end(), vec.begin(), vec.end());

    IMemStream ms(buffer.data(), buffer.size());
    std::ostringstream output;
    cdp::packet::StreamPacketParser parser(ms, output);
    parser.parse();

    str = output.str();
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    EXPECT_EQ(str, "");
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
        std::ifstream file(fileName, std::ios::binary);
        std::ostringstream output;
        cdp::packet::StreamPacketParser parser(file, output);
        parser.parse();
        str = output.str();
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    }
    EXPECT_EQ(str, "B;1;HIGHS;3;0-1S;7;1-2B;7;MEDS;10;2-3S;17;3-2S;21;2-0");
}


TEST(InputStreams, IrrelevantBinFileWithOneError) {
    std::string str("");
    char exe[MAX_PATH];
    GetModuleFileNameA(NULL, exe, MAX_PATH);
    std::string fileName(exe);
    fileName = std::string(exe).substr(0, fileName.find_last_of("\\/"));
    fileName.append("\\IrrelevantBinFile.bin");
    if (std::filesystem::exists(fileName))
    {
        std::ifstream file(fileName, std::ios::binary);
        std::ostringstream output;
        cdp::packet::StreamPacketParser parser(file, output);
        parser.parse();
        str = output.str();
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    }
    EXPECT_EQ(str, "ERR:Wrong battery state value");
}

TEST(InputStreams, RippleStream) {
    std::string str("");

    srand(time(NULL));
    std::vector<char> buffer{};
    for (size_t i = 0; i < 50; ++i) {
        auto& vec = createValidVoltageCurrentPacket(i * 1000, 5, (uint64_t)20 + (rand() % 20) );
        buffer.insert(buffer.end(), vec.begin(), vec.end());
    }

    IMemStream ms(buffer.data(), buffer.size());
    std::ostringstream output;
    cdp::packet::StreamPacketParser parser(ms, output);
    parser.parse();
    str = output.str();
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    EXPECT_EQ(str, "");
}
