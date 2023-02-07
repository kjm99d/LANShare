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
		std::string  dst = "C:\\LanShare\\";
		dst += file_name;

		CBufferWriter buffer_writer;

		SOCKET& sock = client.SOCK;

		// 파일 생성
		CCommandGenerater create_file(PROTOCOL_ID_CREATEFILE, (int)dst.size());
		buffer_writer.Write(sock, create_file); // 헤더 전송
		buffer_writer.Write(sock, (char*)dst.c_str(), dst.size()); // 데이터 전송



		// 파일 버퍼 쓰기
		CFileReader* reader = new CFileReader(4096, (char*)src);
		size_t file_size = reader->FileSize();

		CCommandGenerater WriteFileHeader(PROTOCOL_ID_WRITEFILE, (int)file_size);
		buffer_writer.Write(sock, WriteFileHeader); // 헤더 전송

		while (true)
		{
			// 서버 PC 에서 파일을 읽고
			const char* const file_buf = reader->GetBuffer();
			const size_t buffer_size = reader->GetBufferSize();

			// 파일이 끝이면 그만 보내라
			if (buffer_size == 0)
				break;


			SOCKET& sock = client.SOCK;
			buffer_writer.Write(sock, (char*)file_buf, buffer_size); // 데이터 전송

		}

		delete reader;

		// 파일 핸들 닫기
		CCommandGenerater close_handle(PROTOCOL_ID_CLOSEHANDLE, 0);
		const char* const b = close_handle.GetBuffer();
		const int sz = close_handle.GetSize();
		send(sock, b, sz, 0);

	}

}

void CTCPServer::SendAll(std::string src, std::string file_name)
{
	std::string  dst = "C:\\LanShare";
	dst += file_name;

	CBufferWriter buffer_writer;

	// 졸려죽겠다 .. .. .. 
	auto create_file = CProtocolProvider::GetPacket_CreateFile(dst);
	for (auto client : clients)
	{
		SOCKET& sock = client.SOCK;
		buffer_writer.Write(sock, (char *)create_file.data(), create_file.size()); // 헤더 전송
	}


	// 파일 버퍼 쓰기
	ifstream file(src, std::ios_base::binary);
	auto file_size = std::filesystem::file_size(src);
	auto WriteFileHeader = CProtocolProvider::GetPacket_WriteFile(file_size);

	for (auto client : clients)
	{
		SOCKET& sock = client.SOCK;
		buffer_writer.Write(sock, (char*)WriteFileHeader.data(), WriteFileHeader.size()); // 헤더 전송
	}

	while (true)
	{
		unsigned char buffer[4096] = { 0, };
		std::streamsize gcount = file.read((char *)buffer, 4096).gcount();

		for (auto client : clients)
		{
			SOCKET& sock = client.SOCK;
			buffer_writer.Write(sock, (char*)buffer, gcount); // 데이터 전송
		}

		if (file.eof())
			break;
	}

	// 파일 핸들 닫기
	//CCommandGenerater close_handle(PROTOCOL_ID_CLOSEHANDLE, 0);
	auto CloseFileHeader = CProtocolProvider::GetPacket_CloseFile();
	for (auto client : clients)
	{
		SOCKET& sock = client.SOCK;
		buffer_writer.Write(sock, (char*)CloseFileHeader.data(), CloseFileHeader.size()); // 헤더 전송
	}
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
	for (CLIENT_INFOMATION& info : clients)
	{
		CCommandGenerater cmd(PROTOCOL_ID_HEARTBEAT, 0);
		CBufferWriter writer;
		SafeSend(info.SOCK, (char*)cmd.GetBuffer(), cmd.GetSize());

		std::string  RECV_BUFFER;
		SafeRecv(info.SOCK, RECV_BUFFER);
		reponsebody.append(info.address());
		reponsebody.append("/");
		reponsebody.append(std::to_string (info.port()));
		reponsebody.append("/");

		if (RECV_BUFFER.size() > 0 && RECV_BUFFER[0] == 0x01)
			reponsebody.append("1\r\n");
		else
			reponsebody.append("0\r\n");
	}
}

/**
 * \brief 지정된 클라이언트에게 메세지박스를 보여준다.
 * 
 * \param msg - 보내고자 하는 메세지 내용
 * \param targets - 기본값은 {}, 메세지를 전달받을 특정 클라이언트 리스트
 */
void CTCPServer::Echo(std::string  msg, vector<CLIENT_INFOMATION> targets)
{
	vector<CLIENT_INFOMATION>& target_clients = (targets.size() == 0) ? clients : targets;


	for (CLIENT_INFOMATION& info : target_clients)
	{
		CCommandGenerater cmd(PROTOCOL_ID_ECHO, msg.size());
		SafeSend(info.SOCK, (char*)cmd.GetBuffer(), cmd.GetSize());
		SafeSend(info.SOCK, (char*)msg.c_str(), msg.size());
	}

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
