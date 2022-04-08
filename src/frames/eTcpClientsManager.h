#pragma once
#include "eDataLayer.h"
#include "ePackage.h"
#include "eTcpSrvLayer.h"
#include "ModuleBase.h"

class eTcpClientsManager : public eDataLayer<eSocketShareData>::I_Process_Data_Base{ // ModuleBase<eSocketShareData>{

public:
    eTcpClientsManager();
    virtual ~eTcpClientsManager();

    virtual void Start();
    virtual void hadleNotifyMsg(std::shared_ptr<eSocketShareData>) override;

    virtual void SendData(const unsigned int key, const unsigned char* msg, const unsigned int msg_len, void* user_args = nullptr);
    virtual void CloseRemote(const unsigned int key, void* user_args = nullptr);

    virtual void notify_send_status(unsigned int, std::string&, void*) = 0;
    virtual void notify_close(unsigned int, std::string&, void*) = 0;
    

protected:
    void notify_send_status_cb(unsigned int, std::string, void*);
    void notify_close_cb(unsigned int, std::string, void*);

private:
    eTcpSrvLayer _tcp_srv;
    eDataLayer<eSocketShareData> _gule_self;

};