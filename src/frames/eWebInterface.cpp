#include "eWebInterface.h"
#include <curl/curl.h>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>

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

eWebInterface::eWebInterface(){

}


eWebInterface::~eWebInterface(){

}

void eWebInterface::Init(){
    curl_global_init(CURL_GLOBAL_WIN32);
}