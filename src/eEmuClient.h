#pragma once
#include "frames/ePackage.h"

class eEmuClient{
public:
    eEmuClient();
    virtual ~eEmuClient();

    void DoRecvData(std::shared_ptr<eSocketShareData> sp);
    
};