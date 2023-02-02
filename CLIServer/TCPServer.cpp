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
		const CLIENT_INFOMATION & client = clients[i];
		
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

void CTCPServer::SendAll(const char * src, const char * file_name)
{
	std::string dst = "C:\\LanShare\\";
	dst += file_name;

	CBufferWriter buffer_writer;

	// 파일 생성
	CCommandGenerater create_file(PROTOCOL_ID_CREATEFILE, (int)dst.size());
	
	for (auto client : clients) 
	{
		SOCKET& sock = client.SOCK;
		buffer_writer.Write(sock, create_file); // 헤더 전송
		buffer_writer.Write(sock, (char*)dst.c_str(), dst.size()); // 데이터 전송
	}


	// 파일 버퍼 쓰기
	CFileReader* reader = new CFileReader(4096, (char*)src);
	size_t file_size = reader->FileSize();

	CCommandGenerater WriteFileHeader(PROTOCOL_ID_WRITEFILE, (int)file_size);
	for (auto client : clients)
	{
		SOCKET& sock = client.SOCK;
		buffer_writer.Write(sock, WriteFileHeader); // 헤더 전송
	}

	while (true)
	{
		// 서버 PC 에서 파일을 읽고
		const char* const file_buf = reader->GetBuffer();
		const size_t buffer_size = reader->GetBufferSize();

		// 파일이 끝이면 그만 보내라
		if (buffer_size == 0)
			break;

		for (auto client : clients)
		{
			SOCKET& sock = client.SOCK;
			buffer_writer.Write(sock, (char*)file_buf, buffer_size); // 데이터 전송
		}
	}

	delete reader;

	// 파일 핸들 닫기
	CCommandGenerater close_handle(PROTOCOL_ID_CLOSEHANDLE, 0);


	const char* const b = close_handle.GetBuffer();
	const int sz = close_handle.GetSize();
	for (auto client : clients)
	{
		SOCKET& sock = client.SOCK;
		send(sock, b, sz, 0);
	}
}

void CTCPServer::HeartBeat(string & responseBody)
{
	for(CLIENT_INFOMATION& info : clients)
	{ 
		CCommandGenerater cmd(PROTOCOL_ID_HEARTBEAT, 0);
		CBufferWriter writer;
		//writer.Write(info.SOCK, cmd);
		SafeSend(info.SOCK, (char *)cmd.GetBuffer(), cmd.GetSize());
		SafeRecv(info.SOCK, responseBody);

		//SafeRecv()
	}
}
