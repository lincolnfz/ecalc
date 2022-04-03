
#include <string>


class CRSASign
{
public:
	static CRSASign& GetInstance();
	std::string RSASign(const wchar_t* strText);
	std::string RSASign(const char* strText);
protected:
	CRSASign();
	~CRSASign();
	void* m_pRSA;
	int m_nSigLen;
};