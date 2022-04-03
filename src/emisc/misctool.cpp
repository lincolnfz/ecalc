#include "misctool.h"
#include "../3rd/uuid/uuid.h"
#include <assert.h>

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