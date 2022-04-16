#pragma once
#include "frames/ePackage.h"
#include "./emisc/etypedefine.h"
#include <string.h>
#include <vector>
#include "./emisc/FunDelegate.h"
#include "./frames/PingPongFn.h"
#include "DataPacket.h"

class eEmuManager;
class eEmuClient : public ResponseHandle_0x<TClsMemFnDelegate_1Param<void, char*>>,
                     public AckHandle_0x<TClsMemFnDelegate_1Param<void, char*>>{
public:
    enum STATUS{
        STATUS_FAIL = 0,
        STATUS_LAUNCH = 1,
        STATUS_RESTART = 1,
    };
    eEmuClient(unsigned int key, eEmuManager* manager);
    virtual ~eEmuClient();

    void DoRecvData(std::shared_ptr<eSocketShareData> sp);

    void testreqresponse(int sign, char* json);
protected:
    void testresponse(char* json);
    void testresponse22(char* json);

    //向外发送数据
    void OutData(char* data, unsigned int len);

private:
    CDataPacket _datapacket;
    eEmuManager *_manager;
    unsigned int _key;
    STATUS _status;
};