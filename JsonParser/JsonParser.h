#pragma once
#include<rapidjson\document.h>
#include<iomanip>
#include <curl\curl.h>
#include<fstream>
using namespace rapidjson;
using namespace std;


class ConnectionHandler;

class JsonParser
{
public:
	JsonParser( ConnectionHandler* connectionHandler);
	JsonParser(const JsonParser&) = delete;
	JsonParser& operator = (const JsonParser&) = delete;
	bool SetDocument();
	Document& GetDocument() { return m_document; }
	void PrintColoumnHeader(const Value& jsonValue);
	void RecursiveJsonParser(const Value& jsonValue);
	ConnectionHandler* GetConnectionHandler() const { return m_pConnectionHandler; }
	void PrintColumnValToConsole(const Value& jsonValue);
	std::ofstream& GetCSVFileStream() { return m_fileStream; }

	~JsonParser() 
	{ 
		delete m_pConnectionHandler; 
		m_fileStream.close();
	}
private :
	ConnectionHandler * m_pConnectionHandler = nullptr;
	Document m_document;
	std::ofstream m_fileStream;
};

// class to call URL using Curl lib
class ConnectionHandler
{
public:
	ConnectionHandler()
	{
		m_pCurl = curl_easy_init();
	}
	ConnectionHandler(const ConnectionHandler&) = delete;
	int operator = (const ConnectionHandler&) = delete;
	void HttpRequest();
	//size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data);
	CURL* GetCurl() const { return m_pCurl; }
	string& GetStrResponse() { return m_strResponse; }

	~ConnectionHandler()
	{
		/* always cleanup */
		curl_easy_cleanup(m_pCurl);
	}

private:
	CURL * m_pCurl = nullptr;
	CURLcode m_resCode;
	std::string m_strResponse;
	std::string m_strHeader;
	
};