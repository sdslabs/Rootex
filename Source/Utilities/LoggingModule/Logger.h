#pragma once

namespace std
{
	class iostream;
	class string_view;
}

class Logger
{
private:

	std::iostream &m_Stream;

public:

	void Print(std::string &File, unsigned int &Line, std::string &FormatString, ...);

	Logger(const std::ostream &OutputStream);
	virtual ~Logger();

protected:
};
