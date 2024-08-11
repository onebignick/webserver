#include "HttpRequest.hpp"
#include <iostream>
#include <utility>

using namespace Http;

HttpRequest::HttpRequest(char* buffer, unsigned int buffer_len) : method(), path(), version(), headers(), body()
{
	std::string request_line = "";
	int cur = 0;
	while (buffer[cur] != '\n' && cur < buffer_len)
	{
		request_line += buffer[cur];
		cur++;
	}
	cur++;
	ParseRequestLine(request_line);
	
	std::unordered_map<std::string, std::string> headers_;
	int prev = cur-1;
	bool key = true;
	std::pair<std::string, std::string> key_value;

	while (cur < buffer_len) 
	{
		std::cout << buffer[cur];
		if (buffer[cur] == '\n' && buffer[prev] == '\n')
		{
			break;
		}
		else if (buffer[cur] == '\n')
		{
			cur++;
			prev++;
		}
		else if (buffer[prev] == '\n')
		{
			key = !key;
			headers_.insert(key_value);
			key_value.first = "";
			key_value.second = "";
			cur++;
			prev++;
		}
		else if (buffer[cur] == ' ' && buffer[prev] == ':')
		{
			key = !key;
			cur ++;
			prev ++;
		}
		else
		{
			if (key)
			{
				key_value.first += buffer[prev];
			}
			else
			{
				key_value.second += buffer[prev];
			}
			cur++;
			prev++;
		}
	}

	headers = headers_;

	cur++;
	
	while (cur < buffer_len) 
	{
		body += buffer[cur];
	}
	std::cout << "Body: " << body << std::endl;
	
}

HttpRequest::~HttpRequest() {}

void HttpRequest::ParseRequestLine(std::string request_line)
{
	int cur = 0;
	for (int i=0; i < request_line.length(); i++)
	{
		char to_add = request_line[i];
		if ( request_line[i] == ' ') cur++;
		else if (cur == 0) method += to_add;
		else if (cur == 1) path += to_add;
		else if (cur == 2) version += to_add;
	}
	std::cout << "Method: " << method << std::endl;
	std::cout << "Path: " << path << std::endl;
	std::cout << "Version: " << version << std::endl;
}
