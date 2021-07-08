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
        DynamicJsonDocument getRecommendSongs() const;
        DynamicJsonDocument getUserPlaylists(int uid) const;
        DynamicJsonDocument getPlaylistDetail(int id, int n) const;
        DynamicJsonDocument getUserLevel() const;
        DynamicJsonDocument getUserRecord(int uid, int type) const;
        DynamicJsonDocument getUserFollows(int uid, int offset, int limit) const;
        DynamicJsonDocument getUserFolloweds(int uid, int offset, int limit) const; 
        DynamicJsonDocument getUserEvent(int uid, int lasttime, int limit) const;
        DynamicJsonDocument getPersonalFM() const;

    };

    User login(const String& phone, const String& password);



}
#endif
