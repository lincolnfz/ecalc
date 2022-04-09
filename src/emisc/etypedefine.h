#pragma once

using BYTE = unsigned char;

using WORD = unsigned short;

using DWORD = unsigned long;

#define  SAFE_RELEASE(ptr) if(ptr){free(ptr);} ptr = nullptr;
