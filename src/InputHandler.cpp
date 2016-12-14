#include "InputHandler.h"

#include <iostream>
#include <string>

InputHandler::InputHandler(bool e)
	: debug(e)
{}

InputHandler::InputHandler(int c, char **v, bool e)
	: debug(e)
{
	std::cout << "[InputHandler]-> Parsing input for executable: " << v[0] << std::endl;
	for (int i = 1; i < c; ++i)
	{
		if (v[i][0] == '-')
		{
			std::string s(v[i]);
			s.erase(0, 1); // delete "-"
			options.insert(s);
			if (debug)
				std::cout << "[Debug][InputHandler] Adding option: " << s << std::endl;
		}
		else
		{
			arguments.push_back(v[i]);
			if (debug)
				std::cout << "[Debug][InputHandler] Adding argument: " << v[i] << std::endl;
		}
	}

	if (options.empty() && !arguments.empty())
	{
		options.insert(defaultOption);
		if (debug)
			std::cout << "[Debug][InputHandler] Adding default option: " << defaultOption << std::endl;
	}
		
	arg = arguments.begin();
}

bool InputHandler::option(const std::string &opt) const
{
	return options.find(opt) != options.end();
}

bool InputHandler::option(const char *opt) const
{
	return option(std::string(opt));
}

bool InputHandler::get(std::string &argument)
{
	if (arg != arguments.end())
	{
		argument = *arg;
		arg++;
		return true;
	}
	return false;
}


InputHandler::~InputHandler()
{
}
