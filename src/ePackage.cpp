#include "ePackage.h"
#include <mutex>
#include <string.h>


#define min(a,b) ( (a) < (b) )? (a):(b)
#define max(a,b) ( (a) > (b) )? (a):(b)     
 
/******************************************************************************
 *函数名   :ring_buf_init
 *函数功能 :构造一个空环形缓冲区
 *输入参数 :r 环形缓冲区控制块
 *返回值   :0表示成功 
 *****************************************************************************/
int ring_buf_create(ring_buf_t *r,unsigned char *buf,unsigned int len)
{
    r->buf = buf;
    r->size = len;
    r->front = r->rear = 0;
    return buf == nullptr;
}

/******************************************************************************
 *函数名   :ring_buf_realloc
 *函数功能 :重新构造一个空环形缓冲区
 *输入参数 :r 环形缓冲区控制块
 *返回值   :0表示成功 
 *****************************************************************************/
int ring_buf_realloc(ring_buf_t *old_r, ring_buf_t* new_r, unsigned char *new_buf,unsigned int new_size){
    if(old_r->size >= new_size){
        //新的ringbuf比原来的小,就不创建
        return -1;
    }
    int ret = ring_buf_create(new_r, new_buf, new_size);
    if(!ret){
        while(true){
            unsigned char szTmp[new_size];
            unsigned int len = ring_buf_get(old_r, szTmp, sizeof(szTmp));
            if(len > 0){
                ring_buf_put(new_r, szTmp, len);
            }else{
                break;
            }
        }
    }
    

    return ret;
}

/**********************************************************************
 *函数名   :ring_buf_clr
 *函数功能 :清空环形缓冲区 
 *输入参数 :r - 待清空的环形缓冲区
 *返回值   :None 
 *********************************************************************/
void ring_buf_clr(ring_buf_t *r)
{
    r->front = r->rear = 0;
}
 
/**********************************************************************
 *函数名   :ring_buf_len
 *函数功能 :计算环形缓冲区有效容量 (字节为单位)
 *输入参数 :r.环形缓冲区控制块
 *返回值   :环形缓冲区中有效字节数 
 *********************************************************************/
unsigned int ring_buf_len(ring_buf_t *r)
{
    return r->rear - r->front;
}

/**********************************************************************
 *函数名   :ring_buf_len
 *函数功能 :计算环形缓冲区剩余容量 (字节为单位)
 *输入参数 :r.环形缓冲区控制块
 *返回值   :环形缓冲区中剩余字节数 
 *********************************************************************/
unsigned int ring_buf_remain(ring_buf_t *r){
    return r->size - ring_buf_len(r);
}
 
/**********************************************************************
 *函数名   :ring_buf_put
 *函数功能 :将指定长度的数据放到环形缓冲区中 
 *输入参数 :buf - 数据缓冲区
 *          len - 缓冲区长度 
 *返回值   :实际放到中的数据 
 *********************************************************************/
unsigned int ring_buf_put(ring_buf_t *r,unsigned char *buf,unsigned int len)
{
    unsigned int i;
    unsigned int space;                      
    space = r->size + r->front - r->rear;
    len = min(len , space);                  /*得到实际写入的数据长度*/
    /*min(空闲空间大小,从real_in开始到缓冲区结尾的空间) -------------*/
    i = min(len, r->size - r->rear % r->size);   
    /*
     * 当len > l时，拷贝buffer中剩余的内容
     *而剩余的大小为len - l 
     */            
    memcpy(r->buf + r->rear % r->size, buf, i); 
    memcpy(r->buf, buf + i, len - i);       
    r->rear += len;     
    return len;
 
}
 
 
/**********************************************************************
 *函数名   :rueueGet
 *函数功能 :从环形缓冲区中读取指定长度的数据 
 *输入参数 :len - 读取长度 
 *输出参数 :buf - 输出数据缓冲区
 *返回值   :实际读取长度 
 *********************************************************************/
unsigned int ring_buf_get(ring_buf_t *r,unsigned char *buf,unsigned int len)
{
    unsigned int i;
    unsigned int space;    
    space = r->rear - r->front;                     
    len = min(len , space);                                
    i = min(len, r->size - r->front % r->size );    
    memcpy(buf, r->buf + r->front % r->size, i);    
    memcpy(buf + i, r->buf, len - i);   
    r->front += len;        
    return len;
}

ePackageBase::ePackageBase(){
    _pRecvBuf = new unsigned char[c_pack_size];
    _pSendBuf = new unsigned char[c_pack_size];
    _ring_recvbuf = new ring_buf_t;
    _ring_sendbuf = new ring_buf_t;
    int ret = ring_buf_create(_ring_recvbuf, _pRecvBuf, c_pack_size);
    ret = ring_buf_create(_ring_sendbuf, _pSendBuf, c_pack_size);
}

ePackageBase::~ePackageBase(){
    delete _ring_recvbuf;
    delete _ring_sendbuf;
    delete [] _pRecvBuf;
    delete [] _pSendBuf;
}

unsigned int ePackageBase::Write_RecvBuf(unsigned char *buf, unsigned int len){
    std::unique_lock<std::mutex> lock(_recvbuf_lock);
    unsigned int put_len = ring_buf_remain(_ring_recvbuf);
    if(put_len < len){
        unsigned int old_size = _ring_recvbuf->size;
        unsigned int new_len = max(old_size + c_pack_size, old_size + len + c_pack_size/2);
        unsigned char *new_buf = new unsigned char[new_len];
        ring_buf_t *new_ring_buf = new ring_buf_t;
        ring_buf_realloc(_ring_recvbuf, new_ring_buf, new_buf, new_len);
        put_len = ring_buf_put(new_ring_buf, buf, len);
        delete _ring_recvbuf;
        delete []_pRecvBuf;
        _ring_recvbuf = new_ring_buf;
        _pRecvBuf = new_buf;
    }else{
         put_len = ring_buf_put(_ring_recvbuf, buf, len);
    }
    return put_len;
}

unsigned int ePackageBase::Read_RecvBuf(unsigned char *buf, unsigned int len){
    std::unique_lock<std::mutex> lock(_recvbuf_lock);
    return ring_buf_get(_ring_recvbuf, buf, len);
}

unsigned int ePackageBase::Write_SendBuf(unsigned char *buf, unsigned int len){
    std::unique_lock<std::mutex> lock(_sendbuf_lock);
    unsigned int put_len = ring_buf_remain(_ring_sendbuf);
    if(put_len < len){
        unsigned int old_size = _ring_sendbuf->size;
        unsigned int new_len = max(old_size + c_pack_size, old_size + len + c_pack_size/2);
        unsigned char *new_buf = new unsigned char[new_len];
        ring_buf_t *new_ring_buf = new ring_buf_t;
        ring_buf_realloc(_ring_sendbuf, new_ring_buf, new_buf, new_len);
        put_len = ring_buf_put(new_ring_buf, buf, len);
        delete _ring_sendbuf;
        delete []_pSendBuf;
        _ring_sendbuf = new_ring_buf;
        _pSendBuf = new_buf;
    }else{
        put_len = ring_buf_put(_ring_sendbuf, buf, len);
    }
    return put_len;
}

unsigned int ePackageBase::Read_SendBuf(unsigned char *buf, unsigned int len){
    std::unique_lock<std::mutex> lock(_sendbuf_lock);
    return ring_buf_get(_ring_sendbuf, buf, len);
}

eSocketShareData::eSocketShareData(){
    
}

eSocketShareData::~eSocketShareData(){
    
}

void eSocketShareData::RunIfExec(){
    if(_cb.isbind()){
        _cb(_param_1, _param_2);
        _cb.unbind();
    }
}