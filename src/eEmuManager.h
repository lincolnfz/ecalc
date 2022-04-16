#pragma once
#include "eEmuClient.h"
#include "frames/eTcpClientsManager.h"
#include <thread>
#include <unordered_map>


typedef std::unordered_map<unsigned int, std::unique_ptr<eEmuClient>> EmuUnorderMap;

class eEmuManager : public eTcpClientsManager{
public:
    eEmuManager();
    virtual ~eEmuManager();
    virtual std::thread Start() override;
    
    virtual void SendData(const unsigned int key, const unsigned char* msg, const unsigned int msg_len, void* user_args = nullptr) override;
    virtual void CloseRemote(const unsigned int key, void* user_args = nullptr) override;

protected:
    virtual void handleNotifyMsg(std::shared_ptr<eSocketShareData>) override;
    virtual void handleCheckTimer() override;
    virtual void notify_send_status(unsigned int, std::string&, void*) override;
    virtual void notify_close(unsigned int, std::string&, void*) override;

private:
    EmuUnorderMap _Emumap;
};