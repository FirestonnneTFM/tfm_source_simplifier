#pragma once

#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>

#define USE_TERMINAL_COLORS

#ifdef USE_TERMINAL_COLORS
#define COLOR_RESET   "\033[0m"
#define COLOR_BLACK   "\033[30m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#else
#define COLOR_RESET
#define COLOR_BLACK
#define COLOR_RED
#define COLOR_GREEN
#define COLOR_YELLOW
#define COLOR_BLUE
#define COLOR_MAGENTA
#define COLOR_CYAN
#define COLOR_WHITE
#endif

#define TRACE (std::cout << COLOR_WHITE "[" COLOR_CYAN << __func__ << COLOR_WHITE ":" COLOR_GREEN << __LINE__ << COLOR_WHITE "] " COLOR_RESET)
#define trace(what) (TRACE << #what " = " << (what) << std::endl)
void m_fatal(const char *const func, const int line, const char *const msg);
#define fatal(msg) m_fatal(__func__, __LINE__, (msg))

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))