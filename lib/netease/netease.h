#ifndef _NETEASE_H_
#define _NETEASE_H_

#include <WString.h>
#include <ArduinoJson.h>

namespace netease {

    class User {
    private:
        String  _token;
    public:
        User(const String &token) : _token(token) {}
        bool avaliable() {
            return _token.length() > 0;
        }

        String token() {
            return _token;
        }

        DynamicJsonDocument getUserAccount() const;

    };

    User login(const String& phone, const String& password);



}
#endif
