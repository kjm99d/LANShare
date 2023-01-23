#include "ProtocolV1.h"

/**
 * 문자열 형태로 들어온 메세지 멤버 메세지로 지정한다.
 * 
 * \param message 실제 패킷 정보 (패킷의 끝은 NULL로 처리되어야함)
 */
void CProtocolV1::SetMessage(string message)
{
	this->message = message;
}

/**
 * 멤버변수 message 값을 파싱한다.
 * 
 */
bool CProtocolV1::Parse()
{
	// 메세지가 없으면 [실패]
	const string& msg = this->message;
	if (0 == message.size() || true == message.empty())
		return false;
	
	// 라인수가 0이하 일 경우 파싱 실패
	// 라인수가 2보다 작을 경우 파싱 실패
	// ㄴ 첫줄은 URL 정보, 마지막은 POST일 경우 페이로드 정보
	vector<string> lines = streamTokenizer(msg, '\n');
	if (0 >= lines.size() || 2 > lines.size() )
		return false;

	
	// 첫줄의 데이터 갯수가 3개가 아닌 경우 실패
	vector<string> tokens = streamTokenizer(lines[0], ' ');
	if (tokens.size() != 3)
		return false;

	this->method = tokens[0];
	this->protocol = tokens[2];
	this->resource = tokens[1];
	this->querystring = lines[lines.size() - 1];

	return true;
}

/**
 * 현재 프로토콜에 맞는 이벤트를 실행한다..
 * 
 */
void CProtocolV1::Start()
{
	if (protocol.compare("http") == 0)
	{

	}

}

const vector<string> CProtocolV1::streamTokenizer(string message, char delimeter)
{
	vector<string> ret;

	stringstream stream(message);
	std::string line;
	while (std::getline(stream, line, delimeter))
	{
		ret.push_back(line);
	}
	
	return ret;
}
