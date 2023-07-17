#include "Message.hpp"

std::vector<Message> Message::parse(const std::string &str)
{
	std::vector<Message> message;
	std::vector<std::string> messages;

	messages = split(str, "\\r\\n");
	for (std::vector<std::string>::iterator it = messages.begin(); it != messages.end(); it++)
	{
		message.push_back(parseMessage(*it));
	}

	return message;
}

std::vector<std::string> Message::split(const std::string &str, const std::string &del) 
{
	std::vector<std::string> tokens;
	std::string::size_type pos = 0;

	while (pos != std::string::npos)
	{
		std::string::size_type end = str.find(del, pos);
		if (end == std::string::npos)
		{
			if (str.size() != pos)
			{
				tokens.push_back(str.substr(pos));
			}
			break;
		}

		tokens.push_back(str.substr(pos, end - pos));
		pos = end + del.size();
	}
	return tokens;
}

Message Message::parseMessage(const std::string &str) 
{
	Message message;
	std::istringstream iss(str);
	std::string params;
	std::string token;

	size_t idx = str.find(":", 1);
	if (idx != std::string::npos)
		message.hasTrailing_ = true;
	if (str[0] == ':') {
		iss.ignore(1);
		std::getline(iss, message.prefix_, ' ');
	}
	std::getline(iss, message.command_, ' ');
	std::getline(iss, params, ':');
	std::istringstream iss_params(params);
	while (iss_params >> token)
		message.params_.push_back(token);
	std::getline(iss, message.trailing_);

	return message;
}