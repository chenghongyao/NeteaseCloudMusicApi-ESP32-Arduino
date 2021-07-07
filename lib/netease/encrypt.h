#ifndef NETEASE_ENCRYPT_H_
#define NETEASE_ENCRYPT_H_

#include <WString.h>

namespace netease {

String md5Encode(const String& text);
// char* urlEncode(const char* src, char* dst);
String encryptRequest(const String& data);

}  // namespace netease
#endif
