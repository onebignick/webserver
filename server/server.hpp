#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <unordered_map>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

namespace http 
{

class Request
{
	public:
	std::string method;
	std::string path;
	std::string version;
	std::unordered_map<std::string, std::string> headers;
	std::string static_dir;

	Request(char* buffer, unsigned int buffer_len, std::string static_dir);
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
	struct sockaddr_in address;
	unsigned int address_len;
	
	int startServer();
	void closeServer();
	void acceptConnection(int &incoming_socket);
	void buildResponse(Request req);
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
		{226, "IM Used"}
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
