#include "eTcpClientsManager.h"

eTcpClientsManager::eTcpClientsManager(){

}

eTcpClientsManager::~eTcpClientsManager(){

}

void eTcpClientsManager::Start(){
    _gule_self.Init(&_tcp_srv, this);

    TClsMemFnDelegate_3Param<void, unsigned int, std::string, void*> close_cb;
    close_cb.BindRaw(this, &eTcpClientsManager::notify_close_cb);
    _tcp_srv.RegisterClientClose(close_cb);

    TClsMemFnDelegate_3Param<void, unsigned int, std::string, void*> send_cb;
    send_cb.BindRaw(this, &eTcpClientsManager::notify_send_status_cb);
    _tcp_srv.RegisterSendStatus(send_cb);

    _gule_self.RunMsgPump();
}

void eTcpClientsManager::hadleNotifyMsg(std::shared_ptr<eSocketShareData> msg){

    msg->RunIfExec();

    //ModuleBase<eSocketShareData>::hadleNotifyMsg(msg);
}

void eTcpClientsManager::SendData(const unsigned int key, const unsigned char* msg, const unsigned int msg_len, void* user_args){
    _tcp_srv.SendData2Client(key, msg, msg_len, user_args);
}

void eTcpClientsManager::CloseRemote(const unsigned int key, void* user_args ){
    _tcp_srv.CloseClient(key, user_args);
}

void eTcpClientsManager::notify_send_status_cb(unsigned int key, std::string content, void* user_args){
    notify_send_status(key, content, user_args);
}

void eTcpClientsManager::notify_close_cb(unsigned int key, std::string content, void* user_args){
    notify_close(key, content, user_args);
}