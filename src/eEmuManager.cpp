#include "eEmuManager.h"
#include "./emisc/elog.h"

eEmuManager::eEmuManager(){

}

eEmuManager::~eEmuManager(){

}

void eEmuManager::Start(){
    eTcpClientsManager::Start();
}

void eEmuManager::SendData(const unsigned int key, const unsigned char* msg, const unsigned int msg_len, void* user_args){

    eTcpClientsManager::SendData(key, msg, msg_len, user_args);
}

void eEmuManager::CloseRemote(const unsigned int key, void* user_args){
    ELOG("CloseRemote map count: %d", _Emumap.size());
    eTcpClientsManager::CloseRemote(key, user_args);
}

void eEmuManager::handleNotifyMsg(std::shared_ptr<eSocketShareData> sp){
    //消息队列收到数据
    unsigned int session_id = sp->_session_key;
    EmuUnorderMap::const_iterator got = _Emumap.find(session_id);
    if(got != _Emumap.end()){
        ELOG("handleNotifyMsg exist count: %d", _Emumap.size());
        got->second->DoRecvData(sp);
    }else{
        std::unique_ptr<eEmuClient> client(new eEmuClient(session_id, this));
        std::pair<EmuUnorderMap::iterator, bool> res = _Emumap.emplace(session_id, std::move(client));
        if(res.second){
            res.first->second->DoRecvData(sp);
        }
        ELOG("handleNotifyMsg add count: %d", _Emumap.size());
    }
    
    eTcpClientsManager::handleNotifyMsg(sp);
}

void eEmuManager::handleCheckTimer(){

}

void eEmuManager::notify_send_status(unsigned int key, std::string& content, void* user_args){
    EmuUnorderMap::const_iterator got = _Emumap.find(key);
    if(got != _Emumap.end()){

    }
}

void eEmuManager::notify_close(unsigned int key, std::string& content, void* user_args){
    EmuUnorderMap::const_iterator got = _Emumap.find(key);
    if(got != _Emumap.end()){
        _Emumap.erase(got);
    }
    ELOG("notify_close map count: %d", _Emumap.size());
}

