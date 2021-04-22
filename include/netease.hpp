#ifndef __NETEASE_HPP__
#define __NETEASE_HPP__

#include <StreamString.h>

namespace netease
{

    String login(const String &phone, const String &password);

    DynamicJsonDocument getRecommendSongs(const String &token);
    DynamicJsonDocument getUserPlaylists(const String &token, int uid);
    DynamicJsonDocument getPlaylistDetail(const String &token, int id, int n);
    DynamicJsonDocument getUserDetail(int uid);
    DynamicJsonDocument getUserAccount(const String &token);
    DynamicJsonDocument getUserLevel(const String &token);
    DynamicJsonDocument getMusicUrl(int mid);
    DynamicJsonDocument getUserRecord(int uid, int type, const String &token = "");
    DynamicJsonDocument getUserFollows(int uid, int offset = 0, int limit = 10, const String &token = "");
    DynamicJsonDocument getUserFolloweds(int uid, int offset = 0, int limit = 10, const String &token = "");

    // DynamicJsonDocument getUserEvent(int uid, int lasttime = -1, int limit = 1, const String &token = "");
    DynamicJsonDocument getPersonalFM(const String &token);
}

#endif