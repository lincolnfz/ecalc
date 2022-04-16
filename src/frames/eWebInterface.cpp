#include "eWebInterface.h"
#include <curl/curl.h>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include "../Algorithm/Base64.h"
#include "../Algorithm/des.h"
#include "../emisc/etypedefine.h"

// 秘钥组
std::string GLOBLE_KEY[] = \
{"b588b274-6c62-40d0-b1af-0515346f7974", "6ed13599-f54a-4d02-a30e-da4bc0547450", \
"c99ab386-6d67-455f-a1ac-2a028651a177", "c03800a8-bb76-4b83-bdc5-a3c5825ccf69", \
"6995008e-bba9-47b3-87e3-36e6659412ef", "8eb571ad-749c-4b36-945f-3f78ed657a17", \
"33dad6d6-90d1-49f8-ad59-21acab489389", "0e6e255b-f497-4a16-a94e-adefceeddaf3"};

//#define TEST 1
#ifdef TEST
const BYTE DES_KEY[] = { 20, 99, 100, 99, 111, 53, 101, 102 };
const BYTE DES_VI[] = { 50, 90, 109, 70, 119, 108, 111, 19 };
#else
const BYTE DES_KEY[] = { 88, 27, 25, 80, 111, 36, 77, 91 };
const BYTE DES_VI[] = { 66, 42, 89, 101, 33, 82, 65, 19 };
#endif


std::string url_encode(const std::string& value) {
	std::ostringstream escaped;
	escaped.fill('0');
	escaped << std::hex;

	for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		std::string::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << std::uppercase;
		escaped << '%' << std::setw(2) << int((unsigned char)c);
		escaped << std::nouppercase;
	}

	return escaped.str();
}

CURLcode InitLibCurl(){
	CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
	return res;
}

std::string DesAndBase64Encode(char* pszData)
{
	BYTE* pbyEncrypt = (BYTE*)calloc(strlen(pszData) + 9, 1);
	des_context ctx;
	des_set_key(&ctx, DES_KEY);
	int nEncryptLen = des_cbc_encrypt(&ctx, (const BYTE*)pszData, strlen(pszData), pbyEncrypt, DES_VI);

	int nBase64Len = CBase64::Base64Encode(pbyEncrypt, nEncryptLen, nullptr, 0);
	char* pcsBase64 = (char*)calloc(nBase64Len + 1, sizeof(char));
	CBase64::Base64Encode(pbyEncrypt, nEncryptLen, (BYTE*)pcsBase64, nBase64Len);

	std::string strGet = pcsBase64;
	free(pbyEncrypt);
	free(pcsBase64);

	return strGet;
}

void HttpsPostContent(){
	
}

eWebInterface::eWebInterface(){

}


eWebInterface::~eWebInterface(){

}

void eWebInterface::Init(){
    
}

std::thread eWebInterface::Run(){
	Attach(this);
	std::thread work_thread(&eWebInterface::RunMsgPump, this);
	return work_thread;
}

void eWebInterface::handleNotifyMsg(std::shared_ptr<eWebRequestData> sp){
	int i = 0;
}