#include "common.hpp"
#include <stdlib.h>

void m_fatal(const char *const func, const int line, const char *const msg)
{
	std::cout << COLOR_RED "fatal: " << msg << COLOR_WHITE " (" COLOR_CYAN << func << COLOR_WHITE ":" COLOR_GREEN  << line << COLOR_WHITE ")" COLOR_RESET << std::endl;
	exit(EXIT_FAILURE);
}
