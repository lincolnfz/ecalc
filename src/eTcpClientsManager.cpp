#include "eTcpClientsManager.h"

eTcpClientsManager::eTcpClientsManager(){

}

eTcpClientsManager::~eTcpClientsManager(){

}

void eTcpClientsManager::Start(){
    _gule_srv.Init(&_tcp_srv, this);
    _gule_srv.RunMsgPump();
}

void eTcpClientsManager::hadleNotifyMsg(std::shared_ptr<eSocketShareData> msg){


    ModuleBase<eSocketShareData>::hadleNotifyMsg(msg);
}