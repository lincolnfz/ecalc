#include "misctool.h"
#include "../3rd/uuid/uuid.h"
#include <assert.h>
#include <boost/crc.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

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

std::string generate_guid(){
    boost::uuids::uuid uid = boost::uuids::random_generator()();
    return boost::uuids::to_string(uid);
}