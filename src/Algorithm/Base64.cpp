
#include "Base64.h"
#include <string.h>

CBase64::CBase64()
{
}


CBase64::~CBase64()
{
}

int CBase64::Base64Encode(const void* pSrc, const int nSrcLen, unsigned char* pbyDest, const int nMaxLen)
{
	const char base64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	unsigned char* p1 = (unsigned char*)pSrc;
	unsigned long i, v;
	int nIndex = 0;
	for (i = 0; i < nSrcLen; i += 3)
	{
		if (nIndex + 3 >= nMaxLen)
		{
			break;
		}
		switch (nSrcLen - i)
		{
		case 1:
			v = (p1[i] << 16);
			pbyDest[nIndex++] = base64_alphabet[v >> 18];
			pbyDest[nIndex++] = base64_alphabet[(v >> 12) & 63];
			pbyDest[nIndex++] = base64_alphabet[64];
			pbyDest[nIndex++] = base64_alphabet[64];
			break;

		case 2:
			v = (p1[i] << 16) | (p1[i + 1] << 8);
			pbyDest[nIndex++] = base64_alphabet[v >> 18];
			pbyDest[nIndex++] = base64_alphabet[(v >> 12) & 63];
			pbyDest[nIndex++] = base64_alphabet[(v >> 6) & 63];
			pbyDest[nIndex++] = base64_alphabet[64];
			break;

		default:
			v = (p1[i] << 16) | (p1[i + 1] << 8) | p1[i + 2];
			pbyDest[nIndex++] = base64_alphabet[v >> 18];
			pbyDest[nIndex++] = base64_alphabet[(v >> 12) & 63];
			pbyDest[nIndex++] = base64_alphabet[(v >> 6) & 63];
			pbyDest[nIndex++] = base64_alphabet[v & 63];
			break;
		}
	}
	if (pbyDest && nIndex < nMaxLen)
	{
		pbyDest[nIndex] = '\0';
	}
	return (nSrcLen + 2) / 3 * 4;
}

int CBase64::Base64Encode(const char* lpszEncoding, const int nSrcLen, unsigned char* strEncoded)
{

	int iLen = Base64Encode((unsigned char*)lpszEncoding, nSrcLen, nullptr, 0);

	char *pBuf = new char[iLen + 1];
	memset(pBuf, 0, iLen + 1);
	Base64Encode(lpszEncoding, nSrcLen, (unsigned char*)pBuf, iLen);

	memcpy((char*)strEncoded, pBuf, iLen);

	delete[]pBuf; pBuf = nullptr;
	return iLen;
}

int CBase64::Base64Decode(const char* lpszDecoding, const int nSrcLen, unsigned char* strDecoded)
{
	int iLen = Base64Decode((unsigned char*)lpszDecoding, nSrcLen, nullptr, 0);

	char* pBuf = new char[iLen + 1];
	memset(pBuf, 0, iLen + 1);
	Base64Decode((unsigned char*)lpszDecoding, nSrcLen, pBuf, iLen);

	memcpy((char*)strDecoded, pBuf, iLen);

	delete[]pBuf; pBuf = nullptr;
	return iLen;
}

int CBase64::Base64Decode(const unsigned char* pbySrc, const int nSrcLen, void* pDest, const int nMaxLen)
{
	const char base64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	unsigned char *p2 = (unsigned char *)pDest;
	unsigned long i, v, n;
	unsigned char base64_table[256];

	for (i = 0; i < sizeof(base64_table); i++)
		base64_table[i] = 255;

	for (i = 0; i < 64; i++)
		base64_table[base64_alphabet[i]] = (char)i;

	for (i = 0, n = 0; i < nSrcLen; i++)
	{
		if (base64_table[pbySrc[i]] == 255)
			break;

		if ((unsigned long)(p2 - (unsigned char *)pDest) >= nMaxLen)
			break;

		v = base64_table[pbySrc[i]] | (v << 6);
		n += 6;

		if (n >= 8)
		{
			n -= 8;
			*p2++ = (unsigned char)(v >> n);
		}
	}
	//����ĩβ�м��� = ��( Base64ת���� �� =����Encodeʱ��ĩ����ʱ��׷�ӵ��ַ�) 
	int nEndCount = 0;
	for (int j = nSrcLen - 1; j >= 0; j--)
	{
		if (pbySrc[j] == base64_alphabet[64])
		{
			nEndCount++;
		}
		else
		{
			break;
		}
	}

	return nSrcLen / 4 * 3 - nEndCount;
}