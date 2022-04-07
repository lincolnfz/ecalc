#pragma once
#include "eDataLayer.h"
#include "ePackage.h"
#include "eTcpSrvLayer.h"
#include "ModuleBase.h"

class eTcpClientsManager : public ModuleBase<eSocketShareData>{

public:
    eTcpClientsManager();
    virtual ~eTcpClientsManager();

    void Start();
    virtual void hadleNotifyMsg(std::shared_ptr<eSocketShareData>) override;

private:
    eTcpSrvLayer _tcp_srv;
    eDataLayer<eSocketShareData> _gule_srv;

};