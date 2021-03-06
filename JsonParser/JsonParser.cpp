// JsonParser.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#define CURL_STATICLIB

#include<JsonParser.h>
#include <iostream>
#include<rapidjson\document.h>
#include<iomanip>
#include <curl\curl.h>
#include<stdio.h>
#include<string>
#include<fstream>
#include<direct.h>
#include <exception>

using namespace std;
using namespace rapidjson;

#define COLOMN_ITR 1


JsonParser::JsonParser(ConnectionHandler* connectionHandler)
{
	m_pConnectionHandler = connectionHandler;
}


void JsonParser::PrintColoumnHeader(const Value& jsonValue)
{
	if (jsonValue.IsArray())
	{
		for (int i = 0; i < COLOMN_ITR; i++)
		{
			for (Value::ConstMemberIterator itrMem = jsonValue[i].MemberBegin(); itrMem != jsonValue[i].MemberEnd(); itrMem++)
			{
				m_fileStream << left << itrMem->name.GetString() << ",";
			}

			m_fileStream << "\n";
		}

	}
	else
	{
		for (Value::ConstMemberIterator itrMem = jsonValue.MemberBegin(); itrMem != jsonValue.MemberEnd(); itrMem++)
		{
			m_fileStream << left << itrMem->name.GetString() << ",";
		}

		m_fileStream << "\n";
	}

	cout << "-------------------------------------------------------------------------------------------------------------------------- \n" << endl;
}

//for printing to console but formatting is not good.
void JsonParser::PrintColumnValToConsole(const Value& jsonValue)
{
	if (jsonValue.IsString())
	{
		m_fileStream << left << jsonValue.GetString() << ",";
	}
	else if (jsonValue.IsNumber())
	{
		m_fileStream << left << jsonValue.GetInt() << ",";
	}
	else if (jsonValue.IsBool())
	{
		m_fileStream << left << jsonValue.GetBool() << ",";
	}
	else if (jsonValue.IsNull())
	{
		m_fileStream << left << "null" << ",";
	}
	else if (jsonValue.IsArray())
	{
		RecursiveJsonParser(jsonValue); // for list within object
	}
}

// for nested json parsing
void JsonParser::RecursiveJsonParser(const Value& jsonValue)
{
	if (jsonValue.IsArray()) // for list of objects 
	{
		for (int i = 0; i < jsonValue.Size(); i++)
		{
			if (jsonValue[i].IsObject())
			{
				for (Value::ConstMemberIterator itrMem = jsonValue[i].MemberBegin(); itrMem != jsonValue[i].MemberEnd(); itrMem++)
				{
					PrintColumnValToConsole(itrMem->value);
				}

				m_fileStream << "\n";
			}
			else // for list of non object elements
			{
				PrintColumnValToConsole(jsonValue[i]);
			}
		}
	}
	else // for no list condition, only single object 
	{
		for (Value::ConstMemberIterator itrMem = jsonValue.MemberBegin(); itrMem != jsonValue.MemberEnd(); itrMem++)
		{
			PrintColumnValToConsole(itrMem->value);
		}

		m_fileStream << "\n";
	}
}

bool JsonParser::SetDocument()
{
	bool bResult = true;
	m_document.Parse(m_pConnectionHandler->GetStrResponse().c_str());

	if (m_document.HasParseError())
	{
		cout << "Parse error";

		bResult = false;
	}

	if (m_document.Empty())
	{
		cout << "empty Json";
		bResult = false;
	}

	return bResult;
}

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

void ConnectionHandler::HttpRequest()
{

	int httpResCode = 0;
	if (m_pCurl != nullptr) {
		curl_easy_setopt(m_pCurl, CURLOPT_URL, "http://universities.hipolabs.com/search?country=India");

		curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1L);


		curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, writeFunction);
		curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, &m_strResponse);
		curl_easy_setopt(m_pCurl, CURLOPT_HEADERDATA, &m_strHeader);

		m_resCode = curl_easy_perform(m_pCurl);
		curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &httpResCode);

		if (httpResCode != 200)
		{
			throw httpResCode;
		}
		/* Check for errors */
		if (m_resCode != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(m_resCode));
		}
	}

}


int main()
{

	ConnectionHandler* pConnectionHandler = new ConnectionHandler();
	bool bStreamOk = true;
	try
	{
		pConnectionHandler->HttpRequest();
	}
	catch (int e)
	{
		cout << "no response from URL, http error code: " << "\t" << e << endl;
		delete pConnectionHandler; //  freeing up pConnectionHandler in order to stop creation of jsonParser
		pConnectionHandler = nullptr;
		//exit(1);
	}
	catch (...)
	{
		cout << "Default Exception\n";
	}


	if (pConnectionHandler != nullptr) {
		JsonParser jsonParser(pConnectionHandler);


		if (!jsonParser.SetDocument()) 
		{
			return 0;
		}

		const Value& jsonValue = jsonParser.GetDocument();

		try
		{
			jsonParser.GetCSVFileStream().open("JsonOutput.csv");
		}
		catch (const std::exception& e)
		{
			cout << e.what() << endl;
			getchar();
			return 0;
		}
		catch (...)
		{
			cout << "Default Exception";
			getchar();
			return 0;
		}

		

		jsonParser.PrintColoumnHeader(jsonValue);
		jsonParser.RecursiveJsonParser(jsonValue);

		cout << "exported data to : " << _getcwd(NULL, 0) << "\\JsonOutput.csv";
			
	}

	getchar();
}




