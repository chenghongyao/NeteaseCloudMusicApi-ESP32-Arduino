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
    DynamicJsonDocument getMusicUrl(int mid);
}

#endif