#include "elog.h"
#include <spdlog/spdlog.h>

#ifdef _DEBUG
bool g_logTrace = true;
#else
bool g_logTrace = false;
#endif

void elog(const char* format, ...){
    std::thread::id tid = std::this_thread::get_id();
    std::string res;
    char buffer[256];
    va_list args;
    va_start(args, format);
    //使用vsnprintf避免buff不足, 出core
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);                                  
    res = buffer;
    spdlog::info(res);
} 