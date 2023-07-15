#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <string>
#include <vector>
#include <sstream>

class Message {
	private:
		Message parseMessage(const std::string &str);

	public:
		std::string prefix_;
		std::string command_;
		std::vector<std::string> params_;
		std::string trailing_;
		bool hasTrailing_;
		
		std::vector<Message> parse(const std::string &str);
		std::vector<std::string> split(const std::string &str, const std::string &del);
};

#endif