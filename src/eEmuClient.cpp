#include "eEmuClient.h"
#include <cstddef>
#include <cstdlib>
#include <memory>
#include "./emisc/misctool.h"
#include "emisc/etypedefine.h"

#define PACKETHEAD		5

CDataPacket::CDataPacket(void)
{
    m_dwSize = 0;
    m_pBuf = nullptr;
    m_dwUsedSize = 0;
    //m_check_idx = 0;
}


CDataPacket::~CDataPacket(void)
{
    m_dwSize = 0;
    SAFE_RELEASE( m_pBuf );
}

#define min(a,b) ( (a) < (b) )? (a):(b)
#define max(a,b) ( (a) > (b) )? (a):(b)
#define PACK_HEAD_FLAG 0x02
#define PACK_REAR_FLAG 0x03

#define PACK_HEAD_SIZE 1

#define PACK_ORDER_OFFSET 1
#define PACK_ORDER_SIZE 2

#define PACK_TYPE_OFFSET 3
#define PACK_TYPE_SIZE 1

#define PACK_SIZE_OFFSET 4
#define PACK_SIZE_SIZE 2

#define PACK_CONTENT_OFFSET 6

#define PACK_REAR_SIZE 1

inline int CDataPacket::check_pack_head(BYTE* head, unsigned int len){
    int err = -1;
    if(head[0] == PACK_HEAD_FLAG){
        err = 0;
    }
    return err;
}

inline int CDataPacket::CDataPacket::check_pack_no(BYTE* head, unsigned int len, unsigned short *no){
    int err = -1;
    if(len >= PACK_ORDER_OFFSET+PACK_ORDER_SIZE){
        memcpy(no, head+PACK_ORDER_OFFSET, PACK_ORDER_SIZE);
        err = 0;
    }
    return err;
}

inline int CDataPacket::check_pack_type(BYTE* head, unsigned int len, unsigned char *pack_type){
    int err = -1;
    if(len >= PACK_TYPE_OFFSET+PACK_TYPE_SIZE){
        memcpy(pack_type, head+PACK_TYPE_OFFSET, PACK_TYPE_SIZE);
        err = 0;
    }
    return err;
}

inline int CDataPacket::check_pack_size(BYTE* head, unsigned int len, unsigned short *size){
    int err = -1;
    if(len >= PACK_SIZE_OFFSET+PACK_SIZE_SIZE){
        memcpy(size, head+PACK_SIZE_OFFSET, PACK_SIZE_SIZE);
        err = 0;
    }
    return err;
}

inline int CDataPacket::check_pack_content(BYTE* head, unsigned int len, unsigned short dsize, unsigned char **ppData){
    int err = -1;
    _ASSERT(*ppData == nullptr);
    if(len >= PACK_CONTENT_OFFSET+dsize){
        *ppData = (BYTE*)calloc(dsize, 1);
        memcpy(*ppData, head+PACK_CONTENT_OFFSET, dsize);
        err = 0;
    }
    return err;
}

inline int CDataPacket::check_pack_rear(BYTE* head, unsigned int len, unsigned short dsize){
    int err = -1;
    if( len >= PACK_CONTENT_OFFSET+dsize+PACK_REAR_SIZE ){
        if(head[PACK_CONTENT_OFFSET+dsize] == PACK_REAR_FLAG){
            err = 0;
        }else{
            err = -2; //这是错误的包
        }
    }
    return err;
}

/*
PDATACHANNELPACKET CDataPacket::FilterPacket( const BYTE* pbyRecv, const DWORD dwRecvLen )
{
	PDATACHANNELPACKET pRet = NULL;
	const DWORD dwMaxBufSize = 0xFFFF;
    if ( dwRecvLen > 0 )//有接收到数据，需要对 m_pBuf 追加数据
    {
		if ( m_dwSize == 0 )//初始化第一次数据
		{
			m_dwSize = max( PACKET_SIZE*10, dwRecvLen );
			m_pBuf = (BYTE*)calloc( m_dwSize, 1 );
		}

        DWORD dwTmp = m_dwLen + dwRecvLen;
        if ( dwTmp > dwMaxBufSize )
        {
            //清理
            m_dwLen = 0;
            dwTmp = dwRecvLen;
            if ( dwTmp > dwMaxBufSize )//若清理后仍超出，放弃处理，返回NULL
            {
                return nullptr;
            }
        }
        if ( dwTmp > m_dwSize && dwTmp < dwMaxBufSize )//超过缓冲，需要重新开辟空间
        {
            dwTmp = min( dwTmp + PACKET_SIZE, dwMaxBufSize );
            BYTE* pbyTmp = m_pBuf;
            m_pBuf = (BYTE*)calloc( dwTmp, 1 );
            memcpy( m_pBuf, pbyTmp, m_dwLen );
            m_dwSize = dwTmp;
            free( pbyTmp );
            pbyTmp = nullptr;
        }
        
        //追加数据
        memcpy( &m_pBuf[m_dwLen], pbyRecv, dwRecvLen );
        m_dwLen += dwRecvLen;
    }
    //解析是否存在有效的数据包
    int nHead = -1;
    int nEnd = -1;
	bool bError = false;
    for ( int i = 0; i < m_dwLen; i++ )
	{
		nHead = i;
        if ( m_pBuf[i] == PACK_HEAD_FLAG )
        {
			if ( m_dwLen > nHead + PACKETHEAD + 1 )
			{
				//提取包长
				WORD wSendLen = 0;
				memcpy( &wSendLen, &m_pBuf[nHead+1+PACKETHEAD-2], 2 );
				//计算结束位置，看是否是PACK_REAR_FLAG
				if ( m_dwLen > nHead + PACKETHEAD + wSendLen + 1 )
				{
					if ( m_pBuf[nHead + PACKETHEAD + wSendLen + 1] == PACK_REAR_FLAG )
					{
						nEnd = nHead + PACKETHEAD + wSendLen + 1;
					}
					else
					{
						bError = true;//出现异常的包了，对应位置没找到正确的结束符，说明该PACK_HEAD_FLAG是错误的包头，要清理
					}
				}
			}
			break;
        }
    }
    if ( nEnd > nHead  )
    {
		pRet = (PDATACHANNELPACKET)calloc( 1, sizeof(DATACHANNELPACKET) );
        //提取有效包进行处理
		DWORD dwIndex = nHead+1;
		//提取包序号
		memcpy( &pRet->wPacketNo, &m_pBuf[dwIndex], 2 );
		dwIndex+=2;
		//提取命令号
		pRet->byCommandType = m_pBuf[dwIndex];
		dwIndex++;
		//提取包长
		memcpy( &pRet->wDataLen, &m_pBuf[dwIndex], 2 );
		dwIndex+=2;
		//提取包数据
		if ( pRet->wDataLen > 0 )
		{
			pRet->pbyData = (BYTE*)calloc( pRet->wDataLen+2, 1 );//增加2个字节作为结束符'\0'或L'\0'用以方便外部调用处理
			memcpy( pRet->pbyData, &m_pBuf[dwIndex], pRet->wDataLen );
			dwIndex+=pRet->wDataLen;
		}

		//从缓存中清理处理完的指令
        m_dwLen -= (nEnd + 1);
        memcpy( m_pBuf, &m_pBuf[nEnd+1], m_dwLen );
    }
    else if ( nHead >= 0 )//判断并清理冗余的错误数据
    {
		//计算需要清理的冗余数据长度
		int nClear = 0;
		if ( bError )//如果检测到当前包头是错误的，清理nHead+1
		{
			nClear = nHead + 1;
		}
		else if ( nHead > 0 )//包头前的多余数据清理
		{
			nClear = nHead;
		}
        if ( nClear > 0 )//包头前有无效数据
        {
            m_dwLen -= nClear;
            memcpy( m_pBuf, &m_pBuf[nClear], m_dwLen );
        }
    }
    return pRet;
}
*/

std::vector<PDATACHANNELPACKET> CDataPacket::FilterPacket( const BYTE* pbyRecv, const DWORD dwRecvLen ){
    std::vector<PDATACHANNELPACKET> ret;
    if ( dwRecvLen > 0 ){
		if( remain_size() < dwRecvLen ){
            DWORD minimum_request_size  = dwRecvLen - remain_size();
            DWORD calloc_size =  max(minimum_request_size + m_dwSize + 1024, m_dwSize + 8192 );
            BYTE* tmp = (BYTE*)calloc(calloc_size, 1);
            if(m_pBuf){
                memcpy(tmp, m_pBuf, m_dwUsedSize);
                SAFE_RELEASE(m_pBuf);
                m_pBuf = tmp;
            }else{
                m_pBuf = tmp;
            }
            m_dwSize = calloc_size;
        }
        memcpy(m_pBuf + m_dwUsedSize, pbyRecv, dwRecvLen);
        m_dwUsedSize += dwRecvLen;

        unsigned int check_statues = CHECK_STATUS_BLANK;
        DWORD check_idx = 0;
        for(; check_idx < m_dwUsedSize; ){
            BYTE* head = m_pBuf + check_idx;
            unsigned int check_len = m_pBuf +m_dwUsedSize - head;
            if (check_statues == CHECK_STATUS_BLANK) {
                if(check_pack_head(head, check_len) == 0){
                    m_tmp_datachannelpack = new DATACHANNELPACKET;
                    _ASSERT(m_tmp_datachannelpack);
                    check_idx += PACK_HEAD_SIZE;
                    check_statues |= CHECK_STATUS_HEAD;
                    if(check_pack_no(head, check_len, &(m_tmp_datachannelpack->wPacketNo)) == 0){
                        check_idx += PACK_ORDER_SIZE;
                        check_statues |= CHECK_STATUS_NO;
                        if(check_pack_type(head, check_len, &(m_tmp_datachannelpack->byCommandType)) == 0){
                            check_idx += PACK_TYPE_SIZE;
                            check_statues |= CHECK_STATUS_TYPE;
                            if(check_pack_size(head, check_len, &(m_tmp_datachannelpack->wDataLen)) == 0){
                                check_idx += PACK_SIZE_SIZE;
                                check_statues |= CHECK_STATUS_SIZE;
                                if(check_pack_content(head, check_len,
                                 m_tmp_datachannelpack->wDataLen, &(m_tmp_datachannelpack->pbyData)) == 0){
                                     check_idx += m_tmp_datachannelpack->wDataLen;
                                     check_statues |= CHECK_STATUS_CONTENT;
                                     int check_ret = check_pack_rear(head, check_len, m_tmp_datachannelpack->wDataLen);
                                     if( check_ret == 0 ){
                                         check_idx += PACK_REAR_SIZE;
                                         check_statues |= CHECK_STATUS_REAR;
                                     }else if(check_ret == -2){
                                         check_idx += PACK_REAR_SIZE;
                                         check_statues = CHECK_STATUS_INVALID;
                                     }else if(check_ret == -1){
                                         //没有发现包尾巴
                                     }
                                }else{
                                    //没有有效的包内容
                                }
                            }else{
                                //没有找到包长度
                            }
                        }else{
                            //没有找到包类型
                        }
                    }else{
                        //没有查到包编号
                    }
                }else{
                    //没有找到包开始标志
                    ++check_idx;
                    continue;
                }
            }

            if( check_statues == CHECK_STATUS_FULL ){
                _ASSERT(m_tmp_datachannelpack);
                ret.push_back(m_tmp_datachannelpack);
                m_tmp_datachannelpack = nullptr;
                check_statues = CHECK_STATUS_BLANK;
            }
            else if(check_statues == CHECK_STATUS_INVALID){
                _ASSERT(m_tmp_datachannelpack);
                if(m_tmp_datachannelpack){
                    delete m_tmp_datachannelpack;
                    m_tmp_datachannelpack = nullptr;
                }
                check_idx = head - m_pBuf + 1;
                check_statues = CHECK_STATUS_BLANK;
            }else if(check_statues != CHECK_STATUS_BLANK){
                //到尾巴还没收到完整的包,向前挪动
                if(head != m_pBuf){
                    BYTE* tmp = (BYTE*)calloc(check_len, 1);
                    memcpy(tmp, head, check_len);
                    memcpy(m_pBuf, tmp, check_len);
                    m_dwUsedSize = check_len;
                    free(tmp);
                    break;
                }
            }
        } //for end

        if(check_statues == CHECK_STATUS_BLANK || check_statues == CHECK_STATUS_INVALID){
            m_dwUsedSize = 0;
        }
        
    }
    return ret;
}


BYTE* CDataPacket::BuildPacket( const PDATACHANNELPACKET pPacket, DWORD& dwRetLen )
{
    BYTE* pRet = nullptr;
	dwRetLen = PACK_HEAD_SIZE + PACK_ORDER_SIZE + PACK_TYPE_SIZE + PACK_SIZE_SIZE + pPacket->wDataLen + PACK_REAR_SIZE;
    pRet = (BYTE*)calloc( dwRetLen, 1 );
	DWORD dwIndex = 0;
	//包头
    pRet[dwIndex] = PACK_HEAD_FLAG;
	dwIndex += PACK_HEAD_SIZE;
	//包序号
	memcpy( &pRet[dwIndex], &pPacket->wPacketNo, PACK_ORDER_SIZE );
	dwIndex+=PACK_ORDER_SIZE;
	//包命令号
	pRet[dwIndex] = pPacket->byCommandType;
	dwIndex += PACK_TYPE_SIZE;
	//包长
	memcpy( &pRet[dwIndex], &pPacket->wDataLen, PACK_SIZE_SIZE );
	dwIndex+=PACK_SIZE_SIZE;
	//包数据
    if ( pPacket->wDataLen > 0 )
    {
        memcpy( &pRet[dwIndex], pPacket->pbyData, pPacket->wDataLen );
		dwIndex+=pPacket->wDataLen;
    }
	//包尾
    pRet[dwIndex] = PACK_REAR_FLAG;
	dwIndex += PACK_REAR_SIZE;
    return pRet;
}

void CDataPacket::Clear( )
{
    
}

eEmuClient::eEmuClient(){

}

eEmuClient::~eEmuClient(){

}

void eEmuClient::DoRecvData(std::shared_ptr<eSocketShareData> sp){
    unsigned char szTmp[8192] = {0};
    unsigned int len = 0;
    while( len = sp->Read_RecvBuf(szTmp, sizeof(szTmp)), len > 0){
        std::vector<PDATACHANNELPACKET>packlist = _datapacket.FilterPacket(szTmp, len);
    }
}