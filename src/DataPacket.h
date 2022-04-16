#pragma once
#include <memory>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "./emisc/etypedefine.h"

struct DATACHANNELPACKET{
	unsigned short wPacketNo = 0;
	unsigned char byCommandType = 0;
	unsigned char* pbyData = nullptr;
	unsigned short wDataLen = 0;
    //int check_statues = 0;
    DATACHANNELPACKET(){

    };
    ~DATACHANNELPACKET(){
        Release();
    };
	void Release() { if ( pbyData ){ free(pbyData); pbyData = nullptr; } };
};

typedef DATACHANNELPACKET *PDATACHANNELPACKET;

class CDataPacket{
public:
    CDataPacket(void);
    virtual ~CDataPacket(void);

private:
    using CHECK_STATUS = enum {
        CHECK_STATUS_BLANK = 0x0,
        CHECK_STATUS_HEAD = 0x1, //检查到包头
        CHECK_STATUS_NO = 0x2, //检查到包编号
        CHECK_STATUS_TYPE = 0x4, //检查到包类型
        CHECK_STATUS_SIZE = 0x8, //检查到包长度
        CHECK_STATUS_CONTENT = 0x10, //检查到包内容
        CHECK_STATUS_REAR = 0x20, //检查到包尾
        CHECK_STATUS_INVALID = 0x80,  //无效包
        CHECK_STATUS_FULL = (CHECK_STATUS_HEAD|CHECK_STATUS_NO|
                            CHECK_STATUS_TYPE|CHECK_STATUS_SIZE|
                            CHECK_STATUS_CONTENT|CHECK_STATUS_REAR),
    };

protected:
    BYTE* m_pBuf = nullptr;
    DWORD m_dwSize = 0;
    DWORD m_dwUsedSize = 0;
    //DWORD m_check_idx = 0;
    //unsigned int m_check_statues = CHECK_STATUS_BLANK;
    //PDATACHANNELPACKET m_tmp_datachannelpack = nullptr;

    inline DWORD remain_size(){
        return m_dwSize - m_dwUsedSize;
    };

    inline int check_pack_head(BYTE* head, unsigned int len);

    inline int check_pack_no(BYTE* head, unsigned int len, unsigned short *no);

    inline int check_pack_type(BYTE* head, unsigned int len, unsigned char *pack_type);

    inline int check_pack_size(BYTE* head, unsigned int len, unsigned short *size);

    inline int check_pack_content(BYTE* head, unsigned int len, unsigned short dsize, unsigned char **ppData);

    inline int check_pack_rear(BYTE* head, unsigned int len, unsigned short dsize);

public:
    //注意：以指针 BYTE* 返回的这两个函数，返回值需要通过 free 释放
    std::vector<PDATACHANNELPACKET> FilterPacket( const BYTE* pbyRecv, const DWORD dwRecvLen );
    BYTE* BuildPacket( const PDATACHANNELPACKET pPacket, DWORD& dwRetLen );

    void Clear( );
};
