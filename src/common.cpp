#include "common.hpp"
#include <stdlib.h>

void m_fatal(const char *const func, const int line, const char *const msg)
{
	std::cerr << "fatal: " << msg << " (" << func << ':' << line << ')' << std::endl;
	exit(EXIT_FAILURE);
}
