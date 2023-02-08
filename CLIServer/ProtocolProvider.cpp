#include "ProtocolProvider.h"

// 파일 이름 보내는거
std::vector<unsigned char> CProtocolProvider::GetPacket_CreateFile(std::string filepath)
{
    // =========================== //     4       //    4      //   EX)28 // ====================
    // 전체길이 + 커맨드           //전체길이공간 //커맨드공간 // 데이터공간 ====================
    std::vector<unsigned char> packet(sizeof(int) + sizeof(int) + filepath.size());

    //FillProtocol(packet, PROTOCOL_ID_CREATEFILE);
    
    const int nTotalPacket = packet.size();
    std::copy((unsigned char*)&nTotalPacket, ((unsigned char*)&nTotalPacket) + sizeof(int), packet.begin());

    
    constexpr int iProtocolId = PROTOCOL_ID_CREATEFILE;
    std::copy((unsigned char*)&iProtocolId, ((unsigned char*)&iProtocolId) + sizeof(int), packet.begin() + sizeof(int));

    std::copy(filepath.begin(), filepath.end(), packet.begin() + sizeof(int) + sizeof(int));

    return packet;
}

std::vector<unsigned char> CProtocolProvider::GetPacket_WriteFile(unsigned long long filesize)
{
    // =========================== // 전체길이    + 커맨드 + 정보
    std::vector<unsigned char> packet(sizeof(int) + sizeof(int) + sizeof(int) /* sizeof(filesize) 이미지만.. 너무 커 ..  */);

    const int nTotalPacket = packet.size();
    std::copy((unsigned char*)&nTotalPacket, ((unsigned char*)&nTotalPacket) + sizeof(int), packet.begin());


    constexpr int iProtocolId = PROTOCOL_ID_WRITEFILE;
    std::copy((unsigned char*)&iProtocolId, ((unsigned char*)&iProtocolId) + sizeof(int), packet.begin() + sizeof(int));

    // 파일 정보를 추가한다
    const int iFileSize = filesize;
    std::copy((unsigned char*)&iFileSize, ((unsigned char*)&iFileSize) + sizeof(int), packet.begin() + sizeof(int) + sizeof(int));

    return packet; 
}

std::vector<unsigned char> CProtocolProvider::GetPacket_CloseFile()
{
    // =========================== // 전체길이  + 커맨드
    std::vector<unsigned char> packet(sizeof(int) + sizeof(int));

    const int nTotalPacket = packet.size();
    std::copy((unsigned char*)&nTotalPacket, ((unsigned char*)&nTotalPacket) + sizeof(int), packet.begin());


    constexpr int iProtocolId = PROTOCOL_ID_CLOSEHANDLE;
    std::copy((unsigned char*)&iProtocolId, ((unsigned char*)&iProtocolId) + sizeof(int), packet.begin() + sizeof(int));

    // 파일 정보를 추가

    return packet;
}

std::vector<unsigned char> CProtocolProvider::GetPacket_HeaderBeat()
{
    std::vector<unsigned char> packet(sizeof(int) + sizeof(int));

    const int nTotalPacket = packet.size();
    std::copy((unsigned char*)&nTotalPacket, ((unsigned char*)&nTotalPacket) + sizeof(int), packet.begin());


    constexpr int iProtocolId = PROTOCOL_ID_HEARTBEAT;
    std::copy((unsigned char*)&iProtocolId, ((unsigned char*)&iProtocolId) + sizeof(int), packet.begin() + sizeof(int));

    return packet;
}

std::vector<unsigned char> CProtocolProvider::GetPacket_Echo(std::string message)
{
    // =========================== //     4       //    4      //   EX)28 // ====================
       // 전체길이 + 커맨드           //전체길이공간 //커맨드공간 // 데이터공간 ====================
    std::vector<unsigned char> packet(sizeof(int) + sizeof(int) + message.size());

    //FillProtocol(packet, PROTOCOL_ID_CREATEFILE);

    const int nTotalPacket = packet.size();
    std::copy((unsigned char*)&nTotalPacket, ((unsigned char*)&nTotalPacket) + sizeof(int), packet.begin());


    constexpr int iProtocolId = PROTOCOL_ID_ECHO;
    std::copy((unsigned char*)&iProtocolId, ((unsigned char*)&iProtocolId) + sizeof(int), packet.begin() + sizeof(int));

    std::copy(message.begin(), message.end(), packet.begin() + sizeof(int) + sizeof(int));

    return packet;
}
