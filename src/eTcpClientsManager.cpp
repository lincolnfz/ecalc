#include "eTcpClientsManager.h"

eTcpClientsManager::eTcpClientsManager(){

}

eTcpClientsManager::~eTcpClientsManager(){

}

void eTcpClientsManager::Start(){
    _gule_self.Init(&_tcp_srv, this);
    _gule_self.RunMsgPump();
}

void eTcpClientsManager::hadleNotifyMsg(std::shared_ptr<eSocketShareData> msg){

    msg->RunIfExec();

    //ModuleBase<eSocketShareData>::hadleNotifyMsg(msg);
}