﻿#pragma once
#include <string>

using namespace std;
#pragma hdrstop

class IHTTPResponse
{
public:
	virtual ~IHTTPResponse() {};

public:
	virtual string GetResponseBody() = 0;

	string GetStartLine()
	{
		std::ostringstream oss;
		oss << m_httpVersion << " " << m_statusCode << " " << "OK" << "\r\n";
		return oss.str();
	}

	void SetHttpVersion(string version) { m_httpVersion = version; }
	void SetStatusCode(int code) { m_statusCode = code; }
	void SetCORS(string value) { m_cors = value; }

	string GetCORSHeader() {
		string header;

		return header;
	}

protected:
	string m_cors;
	string m_httpVersion;
	int m_statusCode;
	string m_contentType;
	

};

