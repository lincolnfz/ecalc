#pragma once
#include "frames/ePackage.h"

class eEmuClient{
public:
    eEmuClient();
    ~eEmuClient();

    void DoRecvData(std::shared_ptr<eSocketShareData> sp);
    
};