#pragma once
#include "frames/ePackage.h"
#include "./emisc/etypedefine.h"
#include <string.h>

typedef struct _tagDataChannelPacket{
	unsigned short wPacketNo;
	unsigned char byCommandType;
	unsigned char* pbyData;
	unsigned short wDataLen;
	void Init( ){ memset(&wPacketNo, 0, sizeof(_tagDataChannelPacket)); }
	void Release( ) { if ( pbyData ){ free(pbyData); pbyData = nullptr; } }
}DATACHANNELPACKET,*PDATACHANNELPACKET;

#define PACKET_SIZE			4096

class CDataPacket{
public:
    CDataPacket(void);
    virtual ~CDataPacket(void);

protected:
    BYTE* m_pBuf = nullptr;
    DWORD m_dwSize = 0;
    DWORD m_dwUsedSize = 0;
    DWORD m_check_idx = 0;
    unsigned short m_statues = 0; //0无 1检测到包头

    inline DWORD remain_size(){
        return m_dwSize - m_dwUsedSize;
    };

public:
    //注意：以指针 BYTE* 返回的这两个函数，返回值需要通过 free 释放
    PDATACHANNELPACKET FilterPacket( const BYTE* pbyRecv, const DWORD dwRecvLen );
    BYTE* BuildPacket( const PDATACHANNELPACKET pPacket, DWORD& dwRetLen );

    void Clear( );
};


class eEmuClient{
public:
    eEmuClient();
    virtual ~eEmuClient();

    void DoRecvData(std::shared_ptr<eSocketShareData> sp);

private:
    CDataPacket _datapacket;
    
};