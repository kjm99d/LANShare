#pragma once
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

	string GetCORSHeader() 
	{
		/*
		* Access-Control-Allow-Headers: *\r\n
		* Access-Control-Allow-Origin: *\r\n\r\n
		*/
		std::ostringstream oss;

		if (m_cors.size() > 0) 
		{
			oss << "Access-Control-Allow-Headers: " << m_cors << "\r\n";
			oss << "Access-Control-Allow-Origin: " << m_cors << "\r\n";
			
		}

  		return oss.str();
	}

	string GetContentTypeHeader()
	{
		std::ostringstream oss;

		if (m_contentType.size() > 0)
		{
			oss << "Content-Type: " << m_contentType << "\r\n";
		}

		return oss.str();
	}

protected:
	string m_cors;
	string m_httpVersion;
	int m_statusCode;
	string m_contentType;
	

};

