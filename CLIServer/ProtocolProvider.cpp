#include "ProtocolProvider.h"

// 파일 이름 보내는거
std::vector<unsigned char> CProtocolProvider::GetPacket_CreateFile(std::string filepath)
{
    // 길이 + 커맨드 + 이름
    std::vector<unsigned char> packet(sizeof(int) + sizeof(int) + filepath.size());

    //FillProtocol(packet, PROTOCOL_ID_CREATEFILE);
    
    const int nTotalPacket = filepath.size() + sizeof(int);
    std::copy((unsigned char*)&nTotalPacket, ((unsigned char*)&nTotalPacket) + sizeof(int), packet.begin());

    
    constexpr int iProtocolId = PROTOCOL_ID_CREATEFILE;
    std::copy((unsigned char*)&iProtocolId, ((unsigned char*)&iProtocolId) + sizeof(int), packet.begin() + sizeof(int));

    std::copy(filepath.begin(), filepath.end(), packet.begin() + sizeof(int) + sizeof(int));

    return packet;
}

std::vector<unsigned char> CProtocolProvider::GetPacket_WriteFile(int filesize)
{
    std::vector<unsigned char> packet(sizeof(int) + filesize);

    FillProtocol(packet, PROTOCOL_ID_WRITEFILE);
    std::copy((unsigned char*)&filesize, ((unsigned char*)&filesize) + 4, packet.begin() + sizeof(int));

    return packet; 
}

std::vector<unsigned char> CProtocolProvider::GetPacket_CloseFile()
{
    std::vector<unsigned char> packet(4);

    FillProtocol(packet, PROTOCOL_ID_CLOSEHANDLE);

    return packet;
}

void CProtocolProvider::FillProtocol(std::vector<unsigned char> & vector, const int iProtocolId)
{
    std::copy((unsigned char*)&iProtocolId, ((unsigned char*)&iProtocolId) + sizeof(iProtocolId), vector.begin());

}
