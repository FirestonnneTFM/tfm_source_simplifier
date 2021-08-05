#pragma once

#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>

#define TRACE (std::cout << '[' << __func__ << ':' << __LINE__ << "] ")
#define trace(what) (TRACE << #what " = " << (what) << std::endl)
void m_fatal(const char *const func, const int line, const char *const msg);
#define fatal(msg) m_fatal(__func__, __LINE__, (msg))

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))