#pragma once
#include "./emisc/FunDelegate.h"
#include <string>
#include <mutex>

/*环形缓冲区管理器*/
typedef struct
{
     unsigned char *buf;    /*环形缓冲区        */
     unsigned int size;     /*环形缓冲区        */
     unsigned int front;    /*头指针            */
     unsigned int rear;     /*尾指针            */
}ring_buf_t;
 
/*-------------------------外部接口声明----------------------------*/
/*-------------------------ring_buf_t 长度尽量取 2^n  -------------*/ 
int ring_buf_create(ring_buf_t *r,unsigned char *buf,unsigned int size);
 
void ring_buf_clr(ring_buf_t *r);
 
unsigned int ring_buf_len(ring_buf_t *r);
 
unsigned int ring_buf_put(ring_buf_t *r,unsigned char *buf,unsigned int len);
 
unsigned int ring_buf_get(ring_buf_t *r,unsigned char *buf,unsigned int len);


enum PackageType{
    PACKAGE_TYPE_DATA = 0,
    PACKAGE_TYPE_FN = 1,
};


class ePackageBase{
public:
    ePackageBase(){

    };

    virtual ~ePackageBase(){

    };
protected:
    std::mutex _lock;
    //PackageType _type;
};

class eSocketPackage : public ePackageBase{
public:
    eSocketPackage();
    virtual ~eSocketPackage();

private:
    TClsMemFnDelegate_2Param<void, std::string, std::string> _cb;
    unsigned char _szRecvBuf[16384]; //2^14
    unsigned char _szSendBuf[16384]; //2^14
};