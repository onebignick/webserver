#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <unordered_map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "HttpRequest.hpp"

namespace http 
{

class Request
{
	public:
	std::string method;
	std::string path;
	std::string version;
	std::unordered_map<std::string, std::string> headers;

	Request(char* buffer, unsigned int buffer_len);
	~Request();

	private:
	char* buffer;
};

class Server
{
	public:
	Server(std::string ip_address, int port);
	~Server();
	void startListen();
	
	private:
	int server_socket;
	int incoming_socket;
	int port;
	long incoming_message;
	std::string server_message;
	std::string ip_addr;
	std::string template_dir;
	struct sockaddr_in address;
	unsigned int address_len;
	
	int startServer();
	void closeServer();
	void acceptConnection(int &incoming_socket);
	void BuildResponse(Http::HttpRequest req);
	void sendResponse();
};

class Response
{
	public:
	std::string content;
	Response(
		std::string version,
		int status_code,
		std::unordered_map<std::string, std::string> headers,
		std::string body
	);
	~Response();

	private:
	std::unordered_map<int, std::string> status_codes = {
		{100, "Continue"},
		{101, "Switching Protocols"},
		{102, "Processing"},
		{103, "Early Hints"},
		{200, "OK"},
		{201, "Created"},
		{202, "Accepted"},
		{203, "Non-Authoritative Information"},
		{204, "No Content"},
		{205, "Reset Content"},
		{206, "Partical Content"},
		{207, "Multi-Status"},
		{208, "Already Reported"},
		{226, "IM Used"},
		{300, "Multiple Choices"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{303, "See Other"},
		{304, "Not Modified"},
		{307, "Temporary Redirect"},
		{308, "Permanent Redirect"},
		{400, "Bad Request"},
		{401, "Unauthorized"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{429, "Too Many Requests"},
		{500, "Internal Server Error"}
	};
	std::string version;
	int status_code;
	std::unordered_map<std::string, std::string> headers;
	std::string body;
	void buildResponse();
	void buildStatusLine();
	void buildHeaders();
	void buildBody();
};

}

#endif
