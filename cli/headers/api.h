#ifndef API_H
#define API_H

#define API_URL "https://www.finnhub.io/api/v1/"
#define API_KEY "c2l6svaad3if2cn5gigg"
#define TOKEN_URL "&token="
#define MS_API_URL "http://api.marketstack.com/v1/"
#define ACCESS_KEY_URL "?access_key="
#define ACCESS_KEY "d8c359234548676a4856cf13f019e3ed"

char* get_json(char* url, int include_headers);

#endif
