#include <iostream>

#include "inc/exchange.h"

using namespace std;

int main()
{
    Exchange::Packet testPacket;

    testPacket.command = "TEST!";
    testPacket.data = "DATA";
    testPacket.type = Exchange::PACKET_TYPES::INFO_PACKET_TYPE;
    testPacket.isValid = true;

    std::string data = Exchange::PacketConverter::convert(testPacket);

    Exchange::Packet deconvPacket = Exchange::PacketConverter::convert(data);

    std::cout << "Command:" << deconvPacket.command << std::endl
              << "Type:" << deconvPacket.type << std::endl
              << "Data:" << deconvPacket.data << std::endl
              << "Is valid:" << deconvPacket.isValid << std::endl;

    return 0;
}
