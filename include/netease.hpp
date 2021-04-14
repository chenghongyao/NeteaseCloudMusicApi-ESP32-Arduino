#ifndef __NETEASE_HPP__
#define __NETEASE_HPP__

#include <StreamString.h>

namespace netease{
    
    String login(const String &phone, const String &password); 

    DynamicJsonDocument getRecommendSongs(const String &token);
    DynamicJsonDocument getUserPlaylists(const String &token,const String &uid);
    DynamicJsonDocument getPlaylistDetail(const String &token,const String &id,int n);
}


#endif