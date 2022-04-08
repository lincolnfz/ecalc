#pragma once
#include "../emisc/FunDelegate.h"
#include <memory>
#include <string>
#include <mutex>
#include <list>

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
    using CB = TClsMemFnDelegate_3Param<void, unsigned int, std::string, void*>;
    eSocketShareData();
    virtual ~eSocketShareData();

protected:
    struct CB_ENTRY {
        CB_ENTRY(CB p0, unsigned int p1, std::string p2, void* p3){
            cbfun = p0;
            param_1 = p1;
            param_2 = p2;
            param_3 = p3;
        }
        ~CB_ENTRY(){

        }
        CB cbfun;
        unsigned int param_1;
        std::string param_2;
        void* param_3;
    };
    std::list<std::unique_ptr<CB_ENTRY>> _cb_list;
    std::mutex _cb_lock;

public:
    void RunIfExec();
    void PushCB(CB, unsigned int, std::string, void*);
    unsigned int _session_key = 0; //param_1
};