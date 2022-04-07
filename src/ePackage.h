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

int ring_buf_realloc(ring_buf_t *old_r, ring_buf_t* new_r, unsigned char *new_buf,unsigned int new_size);
 
void ring_buf_clr(ring_buf_t *r);
 
unsigned int ring_buf_len(ring_buf_t *r);

unsigned int ring_buf_remain(ring_buf_t *r);
 
unsigned int ring_buf_put(ring_buf_t *r,unsigned char *buf,unsigned int len);
 
unsigned int ring_buf_get(ring_buf_t *r,unsigned char *buf,unsigned int len);


enum PackageType{
    PACKAGE_TYPE_DATA = 0,
    PACKAGE_TYPE_FN = 1,
};

constexpr unsigned int c_pack_size = 8192; //8192 , 16384

class ePackageBase{
public:
    ePackageBase();

    virtual ~ePackageBase();

    unsigned int Write_RecvBuf(unsigned char *buf,unsigned int len);
    unsigned int Read_RecvBuf(unsigned char *buf,unsigned int len);

    unsigned int Write_SendBuf(unsigned char *buf,unsigned int len);
    unsigned int Read_SendBuf(unsigned char *buf,unsigned int len);

protected:
    std::mutex _recvbuf_lock, _sendbuf_lock;
    unsigned char *_pRecvBuf;
    unsigned char *_pSendBuf;
    ring_buf_t *_ring_recvbuf;
    ring_buf_t *_ring_sendbuf;
};

class eSocketShareData : public ePackageBase{
public:
    eSocketShareData();
    virtual ~eSocketShareData();

    void RunIfExec();

public:
    TClsMemFnDelegate_2Param<void, std::string, std::string> _cb;
    std::string _param_1, _param_2;
    unsigned int _session_key = 0;
};