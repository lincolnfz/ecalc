#pragma once

class CBase64
{
public:
	CBase64();
	virtual ~CBase64();

	static int Base64Encode(const void* pSrc, const int nSrcLen, unsigned char* pbyDest, const int nMaxLen);
	static int Base64Encode(const char* lpszEncoding, const int nSrcLen, unsigned char* strEncoded);

	static int Base64Decode(const char* lpszDecoding, const int nSrcLen, unsigned char* strDecoded);
	static int Base64Decode(const unsigned char* pbySrc, const int nSrcLen, void* pDest, const int nMaxLen);
};

