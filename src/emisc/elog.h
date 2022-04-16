#pragma once

void elog(const char* format, ...);

extern bool g_logTrace;

#define ELOG(...) {if(g_logTrace) elog(__VA_ARGS__);}