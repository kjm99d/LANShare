#pragma once
#include <string>
#include <sstream>
#pragma hdrstop

class IHTTPResponse
{
public:
	virtual ~IHTTPResponse() {};

public:
	virtual std::string GetResponseBody() = 0;

	std::string GetStartLine()
	{
		std::ostringstream oss;

		oss << m_httpVersion << " " << m_statusCode << " " << "OK" << "\r\n";

		return oss.str();
	}

	void SetHttpVersion(std::string version) { m_httpVersion = version; }
	void SetStatusCode(int code) { m_statusCode = code; }
	void SetCORS(std::string value) { m_cors = value; }

	std::string GetCORSHeader() 
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

	std::string GetContentTypeHeader()
	{
		std::ostringstream oss;

		if (m_contentType.size() > 0)
		{
			oss << "Content-Type: " << m_contentType << "\r\n";
		}

		return oss.str();
	}

protected:
	std::string m_cors;
	std::string m_httpVersion;
	int m_statusCode;
	std::string m_contentType;
	

};

