#include "eEmuClient.h"
#include <cstddef>
#include <cstdlib>
#include <memory>
#include "./emisc/misctool.h"
#include "eEmuManager.h"
#include "emisc/etypedefine.h"
#include <spdlog/spdlog.h>


eEmuClient::eEmuClient(unsigned int key, eEmuManager* manager){
    _manager = manager;
    _key = key;
    REGISTER_RESPONSE_0x_FUNCTION(eEmuClient, testresponse, 5);
    REGISTER_RESPONSE_0x_FUNCTION(eEmuClient, testresponse22, 7);
}

eEmuClient::~eEmuClient(){

}

void eEmuClient::DoRecvData(std::shared_ptr<eSocketShareData> sp){
    unsigned char szTmp[8192] = {0};
    unsigned int len = 0;
    while( len = sp->Read_RecvBuf(szTmp, sizeof(szTmp)), len > 0){
        std::vector<PDATACHANNELPACKET>packlist = _datapacket.FilterPacket(szTmp, len);
        for( std::vector<PDATACHANNELPACKET>::iterator it = packlist.begin(); it != packlist.end(); ){
            PDATACHANNELPACKET pack = *it;
            if(HandleResponse_0x((int)pack->byCommandType, (char*)pack->pbyData) != FN_HANDLE_OK){
                _ASSERT(false);
            }
            it = packlist.erase(it);
            delete pack;
        }
    }
    //spdlog::info("DoRecvData!!");
    //char sztmp[] = "helllooo  yyysad";
    //_manager->SendData(_key, (BYTE*)sztmp, strlen(sztmp));
    //return;
    if(_status == STATUS_FAIL){
        //invalid device
        _manager->CloseRemote(_key, nullptr);
    }
}

void eEmuClient::testreqresponse(int sign, char* json){
    HandleResponse_0x(sign, json);
}

void eEmuClient::testresponse(char* json){
    int i = 0;
}

void eEmuClient::testresponse22(char* json){
    int i = 0;
}

void eEmuClient::OutData(char* data, unsigned int len){
    _manager->SendData(_key, (unsigned char*)data, len);
}