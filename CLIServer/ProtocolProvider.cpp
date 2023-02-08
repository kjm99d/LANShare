#include "ProtocolProvider.h"

// 파일 이름 보내는거
std::vector<unsigned char> CProtocolProvider::GetPacket_CreateFile(std::string filepath)
{
    const std::vector<unsigned char>& packet = CProtocolChain(PROTOCOL_ID_CREATEFILE).SetMessage(filepath).Build();

    return packet;
}

std::vector<unsigned char> CProtocolProvider::GetPacket_WriteFile(unsigned long long filesize)
{
    const int iFileSize = filesize;
    const std::vector<unsigned char>& packet = CProtocolChain(PROTOCOL_ID_WRITEFILE).SetData(iFileSize).Build();
    
    return packet;
}

std::vector<unsigned char> CProtocolProvider::GetPacket_CloseFile()
{
    const std::vector<unsigned char>& packet = CProtocolChain(PROTOCOL_ID_CLOSEHANDLE).Build();

    return packet;
}

std::vector<unsigned char> CProtocolProvider::GetPacket_HeaderBeat()
{
    const std::vector<unsigned char>& packet = CProtocolChain(PROTOCOL_ID_HEARTBEAT).Build();

    return packet;
}

std::vector<unsigned char> CProtocolProvider::GetPacket_Echo(std::string message)
{
    const std::vector<unsigned char>& packet = CProtocolChain(PROTOCOL_ID_ECHO).SetMessage(message).Build();

    return packet;
}
