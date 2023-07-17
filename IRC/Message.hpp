#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class Message {
	public:
		std::string prefix_;
		std::string command_;
		std::vector<std::string> params_;
		std::string trailing_;
		bool hasTrailing_;

		static std::vector<Message> parse(const std::string &str);

	private:
		static std::vector<std::string> split(const std::string &res, const std::string &del);
		static Message parseMessage(const std::string &str);
};

#endif