#include "server.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <fstream>

namespace
{
	const unsigned int BUFFER_SIZE = 30720;
}

namespace http
{

Server::Server(std::string ip_address, int port) : 
	ip_addr(ip_address),
	port(port),
	server_socket(),
	incoming_socket(),
	address(),
	address_len(sizeof(address)),
	server_message(),
	template_dir("templates")
{
	startServer();
}

Server::~Server()
{
	closeServer();
}

int Server::startServer()
{
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0)
	{
		std::cerr << "Cannot create socket" << std::endl;
		return 1;
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = inet_addr(ip_addr.c_str());
	if (bind(server_socket, (sockaddr*)&address, sizeof(address)) < 0) {
		std::cerr << "Cannot bind socket to address" << std::endl;
		return 1;
	}

	return 0;
}

void Server::startListen()
{
	if (listen(server_socket, 20) < 0 )
	{
		std::cerr << "Socket listen failed" << std::endl;
	}

	unsigned int bytes_received;
	while (true)
	{
		acceptConnection(incoming_socket);

		char buffer[BUFFER_SIZE] = {0};
		bytes_received = read(incoming_socket, buffer, BUFFER_SIZE);
		if (bytes_received < 0)
		{
			std::cerr << "Failed to read bytes from the client" << std::endl;
		}
		std::cout << "Received request" << std::endl;
		std::cout << buffer << std::endl;
		
		Request req = Request((char*)&buffer, bytes_received);
		buildResponse(req);
		sendResponse();
		close(incoming_socket);
	}
}

void Server::acceptConnection(int &incoming_socket)
{
	incoming_socket = accept(server_socket, (sockaddr*)&address, (socklen_t*)&address_len);
	if (incoming_socket < 0) 
	{
		std::cerr << "Server failed to accept incoming connection" << std::endl;
	}
}

void Server::buildResponse(Request req)
{
	std::cout << req.method << " " << req.path << std::endl;
	int status_code;
	std::string resource_location = "./"+template_dir+req.path;
	std::cout << resource_location << std::endl;
	std::ostringstream body;
	if (req.method == "GET")
	{
		std::ifstream infile(resource_location);
		if (infile)
		{
			status_code = 200;
			body << infile.rdbuf();
			infile.close();
		}
		else
		{
			status_code = 404;
			std::ifstream notFound("./"+template_dir+"/404.html");
			body << notFound.rdbuf();
			notFound.close();
		}
	}
	else if (req.method == "POST")
	{
	}

	std::string body_str = body.str();
	std::unordered_map<std::string, std::string> headers = {
		{"Content-Type", "text/html"},
		{"Content-Length", std::to_string(body_str.size())}
	};
	Response res = Response(
		"HTTP/1.1",
		status_code,
		headers,
		body_str
	);
	server_message = res.content;
}

void Server::sendResponse()
{
	long bytes_sent;
	std::cout << "Server Message: " << server_message << std::endl;
	bytes_sent = write(incoming_socket, server_message.c_str(), server_message.size());
	if (bytes_sent == server_message.size())
	{
		std::cout << "Response successfully sent to client" << std::endl;
	}
	else
	{
		std::cout << "Error sending response to client" << std::endl;
	}
}

void Server::closeServer()
{
	close(server_socket);
	close(incoming_socket);
	exit(0);
}

Request::Request(char* buffer, unsigned int buffer_len) :
	method(),
	path(),
	version(),
	buffer(buffer)
{
	std::vector<std::string> v(5);
	int cur = 0;
	for (int i=0;i<buffer_len;i++)
	{
		if (buffer[i] == '\n') break;
		else if (buffer[i] == ' ') cur++;
		else v[cur]+=buffer[i]; 
	}
	method = v[0];
	if (v[1].back()=='/') v[1] += "index.html";
	path = v[1];
	version = v[2];
}

Request::~Request() {}

Response::Response(
	std::string version,
	int status_code,
	std::unordered_map<std::string, std::string> headers,
	std::string body
) :
	version(version),
	status_code(status_code),
	headers(headers),
	body(body)
{
	buildStatusLine();
	buildHeaders();
	buildBody();
};

Response::~Response() {};

void Response::buildStatusLine()
{
	content += version + " " + std::to_string(status_code) + " " + status_codes[status_code] + '\n';
};

void Response::buildHeaders()
{
	for(auto &kv: headers)
	{
		content += kv.first + ": " + kv.second + '\n';
	}
	content += '\n';
};

void Response::buildBody()
{
	content += body;
};

}
