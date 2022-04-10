#pragma once

#include <string>
#include <boost/crc.hpp>

#define GUID_KEY_LEN 37
std::string CreateGuid();

void E_ASSERT(const bool);

#define _ASSERT( expr ) \
            {if(!(expr)){ char szTemp[1024] = {0}; \
            snprintf(szTemp, sizeof(szTemp)-1, "assert(false) %s:%d,  fun: %s,  %s, %s \n", __FILE__, __LINE__ , __FUNCTION__, __DATE__, __TIME__); \
            printf(szTemp); } \
            E_ASSERT(expr); \
            }
            