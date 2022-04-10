#include "misctool.h"
#include "../3rd/uuid/uuid.h"
#include <assert.h>
#include <boost/crc.hpp>

std::string CreateGuid()
{
    uuid_t uu;
    char str[GUID_KEY_LEN];
    uuid_generate( uu );
    uuid_unparse(uu, str);
    return std::string(str);
}

void E_ASSERT(const bool expr){
    #ifdef _DEBUG
		assert(expr);
	#endif
}

void CRC64(const std::string &inputStr)
{
   boost::crc_optimal<64, 0x42F0E1EBA9EA3693,
                      0xffffffffffffffff, 0xffffffffffffffff,
                      false, false> crc;
}