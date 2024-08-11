#include <string>
#include <unordered_map>

namespace Http
{
	class HttpRequest
	{
		public:
		std::string method;
		std::string path;
		std::string version;
		std::unordered_map<std::string, std::string> headers;
		std::string body;
		
		HttpRequest(char* buffer, unsigned int buffer_len);
		~HttpRequest();

		private:
		char* buffer;
		
		void ParseRequestLine(std::string request_line);
	};
}
