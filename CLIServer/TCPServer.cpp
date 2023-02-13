#include "TCPServer.h"

CTCPServer::CTCPServer()
{
}

CTCPServer::~CTCPServer()
{
}

void CTCPServer::ShowClient()
{
	// Mutex 걸어줘야함

	for (int i = 0; i < clients.size(); ++i)
	{
		const int seq = i + 1;
		const CLIENT_INFOMATION& client = clients[i];

		const char* client_addr = inet_ntoa(client.ADDR.sin_addr);
		const u_short client_port = ntohs(client.ADDR.sin_port);

		printf("[%d] 클라이언트 접속 : IP 주소 = %s\t 포트 번호 = %d\n", seq, client_addr, client_port);

	}
}

void CTCPServer::AddClient()
{
	SOCKET sock;
	SOCKADDR_IN addr;
	if (Accept(sock, addr))
	{
		CLIENT_INFOMATION info = { sock, addr };
		clients.push_back(info);
	}


}

int CTCPServer::Receive(fp_TCPEvent cb_callback)
{
	// 콜백 함수 
	if (cb_callback)
	{
		cb_callback(0);
	}

	return 0;
}

void CTCPServer::SendTo(std::string  address, const char* src, const char* file_name)
{

	CLIENT_INFOMATION client;
	if (FindClientFromAddress(address, client) == true)
	{

		std::string  dst = "C:\\LanShare";
		dst += file_name;

		// 파일 생성하기
		auto create_file = CProtocolProvider::GetPacket_CreateFile(dst);
		SafeSend(client.SOCK, (char*)create_file.data(), create_file.size());


		// 파일 버퍼 쓰기
		ifstream file(src, std::ios_base::binary);
		auto file_size = std::filesystem::file_size(src);
		auto WriteFileHeader = CProtocolProvider::GetPacket_WriteFile(file_size);
		SafeSend(client.SOCK, (char*)WriteFileHeader.data(), WriteFileHeader.size());


		unsigned char buffer[4096] = { 0, };
		while (true)
		{
			std::streamsize gcount = file.read((char*)buffer, 4096).gcount();
			SafeSend(client.SOCK, (char*)buffer, gcount);

			if (file.eof()) break;
		}

		// 파일 핸들 닫기
		auto CloseFileHeader = CProtocolProvider::GetPacket_CloseFile();
		SafeSend(client.SOCK, (char*)CloseFileHeader.data(), CloseFileHeader.size());

	}
}

void CTCPServer::SendAll(std::string src, std::string file_name)
{
	std::string  dst = "C:\\LanShare";
	dst += file_name;

	// 파일 생성하기
	auto create_file = CProtocolProvider::GetPacket_CreateFile(dst);
	for (auto client : clients)
		SafeSend(client.SOCK, (char *)create_file.data(), create_file.size());
	

	// 파일 버퍼 쓰기
	ifstream file(src, std::ios_base::binary);
	auto file_size = std::filesystem::file_size(src);
	auto WriteFileHeader = CProtocolProvider::GetPacket_WriteFile(file_size);
	for (auto client : clients)
		SafeSend(client.SOCK, (char*)WriteFileHeader.data(), WriteFileHeader.size());


	unsigned char buffer[4096] = { 0, };
	while (true)
	{
		std::streamsize gcount = file.read((char *)buffer, 4096).gcount();

		for (auto client : clients)
			SafeSend(client.SOCK, (char*)buffer, gcount);

		if (file.eof()) break;
	}

	// 파일 핸들 닫기
	auto CloseFileHeader = CProtocolProvider::GetPacket_CloseFile();
	for (auto client : clients)
		SafeSend(client.SOCK, (char*)CloseFileHeader.data(), CloseFileHeader.size());
}

// ============================================================================================================ //
// 커맨드 관련 정의 함수 목록
// ============================================================================================================ //

/**
 * \brief 연결되어 있는 모든 클라이언트에게 테스트 메세지를 보내고, 클라이언트 상태 정보를 전달 받는다.
 *
 * \param reponsebody - 요청 정보를 전달 받을 파라미터
 */
void CTCPServer::HeartBeat(std::string & reponsebody)
{
	
	const Json::Value items = HeartBeat();

	for (auto item : items)
	{
		reponsebody += item["address"].asString();
		reponsebody += "/";
		reponsebody += item["status"].asString();
		reponsebody += "\n";
	}

	//Json::FastWriter writer;
	//reponsebody = writer.write(items);

}

Json::Value CTCPServer::HeartBeat()
{
	Json::Value result;

	auto HeaderBeat = CProtocolProvider::GetPacket_HeaderBeat();

	for (CLIENT_INFOMATION& info : clients)
	{
		// 패킷 보내고 
		SafeSend(info.SOCK, (char*)HeaderBeat.data(), HeaderBeat.size());

		Json::Value item;
		item["address"] = info.address();
		item["status"] = false;
		std::string  RECV_BUFFER;
		if (TRUE == SafeRecv(info.SOCK, RECV_BUFFER)) { // 패킷을 수신하고
			if (RECV_BUFFER.size() > 0 && RECV_BUFFER[0] == 0x01)
				item["status"] = true;
		}

		result.append(item);
	}

	return result;
}

/**
 * \brief 지정된 클라이언트에게 메세지박스를 보여준다.
 * 
 * \param msg - 보내고자 하는 메세지 내용
 * \param targets - 기본값은 {}, 메세지를 전달받을 특정 클라이언트 리스트
 */
void CTCPServer::Echo(std::string  msg, vector<CLIENT_INFOMATION> targets)
{
	auto Echo = CProtocolProvider::GetPacket_Echo(msg);
	vector<CLIENT_INFOMATION>& target_clients = (targets.size() == 0) ? clients : targets;

	for (CLIENT_INFOMATION& info : target_clients)
		SafeSend(info.SOCK, (char*)Echo.data(), Echo.size());

}


/**
 * \brief 지정된 클라이언트에게 Cmd.exe를 통해 제어하는 명령어를 전달한다.
 * 
 * \param command
 * \param targets
 */
void CTCPServer::CommandLine(std::string command, vector<CLIENT_INFOMATION> targets)
{
	auto commandline = CProtocolProvider::GetPacket_CommandLine(command);
	vector<CLIENT_INFOMATION>& target_clients = (targets.size() == 0) ? clients : targets;

	for (CLIENT_INFOMATION& info : target_clients)
		SafeSend(info.SOCK, (char*)commandline.data(), commandline.size());
}

bool CTCPServer::FindClientFromAddress(std::string  address, CLIENT_INFOMATION& client)
{
	for (int i = 0; i < clients.size(); ++i)
	{
		if (clients[i].address().compare(address) == 0)
		{
			client = clients[i];
			return true;
		}
	}
	return false;

}
