#include "ProtocolProvider.h"

// 파일 이름 보내는거
std::vector<unsigned char> CProtocolProvider::GetPacket_CreateFile(std::string filepath)
{
    const std::vector<unsigned char>& packet = CProtocolChain(PROTOCOL_ID_CREATEFILE).SetMessage(filepath).Build();

    return packet;
}

// 데이터 채워 넣는거
std::vector<unsigned char> CProtocolProvider::GetPacket_WriteFile(uintmax_t filesize)
{
    const std::vector<unsigned char>& packet = CProtocolChain(PROTOCOL_ID_WRITEFILE).SetData(filesize).Build();
    
    return packet;
}

// 파일 핸들 닫아주는거
std::vector<unsigned char> CProtocolProvider::GetPacket_CloseFile()
{
    const std::vector<unsigned char>& packet = CProtocolChain(PROTOCOL_ID_CLOSEHANDLE).Build();

    return packet;
}

// 마 니 뒤졌노 살았노 ?
std::vector<unsigned char> CProtocolProvider::GetPacket_HeaderBeat()
{
    const std::vector<unsigned char>& packet = CProtocolChain(PROTOCOL_ID_HEARTBEAT).Build();

    return packet;
}

// 메박을 띄어보자 ! 
std::vector<unsigned char> CProtocolProvider::GetPacket_Echo(std::string message)
{
    const std::vector<unsigned char>& packet = CProtocolChain(PROTOCOL_ID_ECHO).SetMessage(message).Build();

    return packet;
}

// cmd 명렁어로 실행
std::vector<unsigned char> CProtocolProvider::GetPacket_CommandLine(std::string command)
{
    const std::vector<unsigned char>& packet = CProtocolChain(PROTOCOL_ID_COMMAND_LINE).SetMessage(command).Build();

    return packet;
}
