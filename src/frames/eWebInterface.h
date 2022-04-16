#pragma once
#include "eWorkThread.h"
#include "ePackage.h"

class eWebInterface : public eWorkThread<eWebRequestData>, eWorkThread<eWebRequestData>::I_Notify_CallBadk{
public:
    eWebInterface();
    virtual ~eWebInterface();

    void Init();

    std::thread Run();

protected:
    virtual void handleNotifyMsg(std::shared_ptr<eWebRequestData>);
};