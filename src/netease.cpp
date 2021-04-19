#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "encrypt.hpp"

namespace netease
{

    namespace url
    {

        const char *login =
            "http://music.163.com/weapi/login/cellphone?csrf_token=";
        const char *recommend_songs =
            "http://music.163.com/weapi/v2/discovery/recommend/songs?csrf_token=";
        const char *playlist_detail =
            "https://music.163.com/weapi/v3/playlist/detail";
        const char *user_detail =
            "https://music.163.com/weapi/v1/user/detail";

        const char *user_account =
            "https://music.163.com/api/nuser/account/get";

        const char *user_playlist =
            "https://music.163.com/weapi/user/playlist";
        const char *music_url =
            "http://music.163.com/weapi/song/enhance/player/url";
        const char *user_record =
            "https://music.163.com/weapi/v1/play/record";

    }

    DynamicJsonDocument commandRequest(const String &url, const String &token,
                                       const DynamicJsonDocument &data, const DynamicJsonDocument &filter, int bufferSize = 1024 * 20)
    {
        String strData;
        serializeJson(data, strData);

        DynamicJsonDocument doc(bufferSize);
        HTTPClient client;
        client.begin(url);
        if (!token.isEmpty())
        {
            client.addHeader("cookie", "MUSIC_U=" + token);
        }
        client.addHeader("Content-Type", "application/x-www-form-urlencoded");
        client.setUserAgent(
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
            "like Gecko) Chrome/83.0.4103.97 Safari/537.36");

        int status_code = client.POST(netease::encryptRequest(strData));
        if (status_code != 200)
        {
            Serial.println(status_code);
        }
        else
        {
            DeserializationError error = deserializeJson(
                doc, client.getStream(), DeserializationOption::Filter(filter));
            if (error)
            {
                Serial.println("fail");
                Serial.println(error.f_str());
            }
        }
        client.end();
        return doc;
    }

    DynamicJsonDocument commandRequest(const String &url, const DynamicJsonDocument &data, const DynamicJsonDocument &filter, int bufferSize = 1024 * 20)
    {
        return commandRequest(url, "", data, filter, bufferSize);
    }

    String login(const String &phone, const String &password)
    {
        String res;

        StaticJsonDocument<100> docData;
        docData["phone"] = phone;
        docData["password"] = md5Encode(password);
        docData["rememberLogin"] = true;

        String data;
        serializeJson(docData, data);

        HTTPClient client;
        client.begin(url::login);
        client.addHeader(
            "Cookie",
            "os=pc;appver=2.0.3.131777; channel=netease; __remember_me=true;");
        client.addHeader("Content-Type", "application/x-www-form-urlencoded");
        client.setUserAgent(
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
            "like Gecko) Chrome/83.0.4103.97 Safari/537.36");

        int status_code = client.POST(encryptRequest(data));
        if (status_code == 200)
        {
            DynamicJsonDocument doc(1024 * 5);
            deserializeJson(doc, client.getStream());
            int code = doc["code"].as<int>();
            if (code == 200)
            {
                res = doc["token"].as<String>();
            }
            else
            {
                Serial.print("code error:");
                Serial.println(code);
            }
        }
        else
        {
            Serial.print("http error:");
            Serial.println(status_code);
        }

        client.end();
        return res;
    }

    DynamicJsonDocument getRecommendSongs(const String &token)
    {
        DynamicJsonDocument data(100);
        data["offset"] = 0;
        data["limit"] = 999;
        data["total"] = true;

        DynamicJsonDocument filter(200);
        filter["code"] = true;
        filter["msg"] = true;
        filter["recommend"][0]["name"] = true;
        filter["recommend"][0]["id"] = true;
        filter["recommend"][0]["artists"][0]["name"] = true;
        filter["recommend"][0]["artists"][0]["id"] = true;
        filter["recommend"][0]["album"]["name"] = true;
        filter["recommend"][0]["album"]["id"] = true;

        return commandRequest(url::recommend_songs, token, data, filter);
    }

    DynamicJsonDocument getUserPlaylists(const String &token, int uid)
    {

        DynamicJsonDocument data(100);
        data["offset"] = 0;
        data["limit"] = 99;
        data["uid"] = uid;

        DynamicJsonDocument filter(200);
        filter["code"] = true;
        filter["msg"] = true;
        filter["more"] = true;
        filter["playlist"][0]["name"] = true;
        filter["playlist"][0]["id"] = true;
        filter["playlist"][0]["userId"] = true;
        filter["playlist"][0]["trackCount"] = true;
        filter["playlist"][0]["playCount"] = true;

        return commandRequest(url::user_playlist, token, data, filter);
    }

    // TODO: start??
    DynamicJsonDocument getPlaylistDetail(const String &token, int id, int n)
    {
        DynamicJsonDocument data(100);
        data["id"] = id;
        data["n"] = n;

        DynamicJsonDocument filter(300);
        filter["code"] = true;
        filter["msg"] = true;
        filter["playlist"]["name"] = true;
        filter["playlist"]["id"] = true;
        filter["playlist"]["coverImgUrl"] = true;
        filter["playlist"]["trackCount"] = true;
        filter["playlist"]["playCount"] = true;
        filter["playlist"]["tracks"][0]["name"] = true;
        filter["playlist"]["tracks"][0]["id"] = true;
        filter["playlist"]["tracks"][0]["ar"][0]["name"] = true;
        filter["playlist"]["tracks"][0]["ar"][0]["id"] = true;

        return commandRequest(url::playlist_detail, token, data, filter);
    }

    DynamicJsonDocument getUserDetail(int uid)
    {
        DynamicJsonDocument data(100);

        DynamicJsonDocument filter(300);
        filter["code"] = true;
        filter["msg"] = true;
        filter["level"] = true;
        filter["listenSongs"] = true;
        filter["profile"]["nickname"] = true;
        filter["profile"]["avatarUrl"] = true;
        filter["profile"]["vipType"] = true;
        filter["profile"]["createTime"] = true;
        filter["profile"]["birthday"] = true;
        filter["profile"]["gender"] = true;
        filter["profile"]["province"] = true;
        filter["profile"]["city"] = true;
        filter["profile"]["description"] = true;
        filter["profile"]["followeds"] = true;
        filter["profile"]["follows"] = true;
        filter["profile"]["eventCount"] = true;
        filter["profile"]["playlistCount"] = true;
        filter["peopleCanSeeMyPlayRecord"] = true;
        filter["createDays"] = true;

        String uri = url::user_detail;
        uri = uri + "/" + uid;

        return commandRequest(uri, data, filter);
    }

    DynamicJsonDocument getUserAccount(const String &token)
    {
        DynamicJsonDocument data(100);

        DynamicJsonDocument filter(300);
        filter["code"] = true;
        filter["msg"] = true;
        filter["account"]["id"] = true;
        filter["account"]["userName"] = true;
        filter["account"]["type"] = true;
        filter["account"]["createTime"] = true;
        filter["account"]["vipType"] = true;
        filter["profile"]["userId"] = true;
        filter["profile"]["nickname"] = true;
        filter["profile"]["avatarUrl"] = true;
        filter["profile"]["backgroundUrl"] = true;
        filter["profile"]["vipType"] = true;
        filter["profile"]["createTime"] = true;
        filter["profile"]["birthday"] = true;
        filter["profile"]["gender"] = true;
        filter["profile"]["province"] = true;
        filter["profile"]["city"] = true;
        filter["profile"]["description"] = true;
        filter["profile"]["lastLoginTime"] = true;
        filter["profile"]["lastLoginIP"] = true;

        return commandRequest(url::user_account, token, data, filter);
    }

    DynamicJsonDocument getMusicUrl(int mid)
    {
        DynamicJsonDocument data(100);
        data["br"] = 999000;
        data["csrf_token"] = "";
        data["ids"][0] = mid;

        DynamicJsonDocument filter(100);
        filter["code"] = true;
        filter["msg"] = true;
        filter["data"][0]["id"] = true;
        filter["data"][0]["url"] = true;
        filter["data"][0]["md5"] = true;
        filter["data"][0]["type"] = true;

        return commandRequest(url::music_url, data, filter);
    }

    DynamicJsonDocument getUserRecord(int uid, int type, const String &token)
    {
        DynamicJsonDocument data(100);
        data["uid"] = uid;
        data["type"] = type;

        DynamicJsonDocument filter(300);
        filter["code"] = true;
        filter["msg"] = true;
        if (type == 0)
        {
            filter["allData"][0]["playCount"] = true;
            filter["allData"][0]["score"] = true;
            filter["allData"][0]["song"]["name"] = true;
            filter["allData"][0]["song"]["id"] = true;
            filter["allData"][0]["song"]["ar"][0]["name"] = true;
            filter["allData"][0]["song"]["ar"][0]["id"] = true;
            filter["allData"][0]["song"]["al"][0]["name"] = true;
            filter["allData"][0]["song"]["al"][0]["id"] = true;
            filter["allData"][0]["song"]["dt"] = true;
            filter["allData"][0]["song"]["h"]["br"] = true;
            filter["allData"][0]["song"]["h"]["size"] = true;
            filter["allData"][0]["song"]["m"]["br"] = true;
            filter["allData"][0]["song"]["m"]["size"] = true;
            filter["allData"][0]["song"]["l"]["br"] = true;
            filter["allData"][0]["song"]["l"]["size"] = true;
            filter["allData"][0]["song"]["publishTime"] = true;
        }
        else
        {
            filter["weekData"][0]["playCount"] = true;
            filter["weekData"][0]["score"] = true;
            filter["weekData"][0]["song"]["name"] = true;
            filter["weekData"][0]["song"]["id"] = true;
            filter["weekData"][0]["song"]["ar"][0]["name"] = true;
            filter["weekData"][0]["song"]["ar"][0]["id"] = true;
            filter["weekData"][0]["song"]["al"][0]["name"] = true;
            filter["weekData"][0]["song"]["al"][0]["id"] = true;
            filter["weekData"][0]["song"]["dt"] = true;
            filter["weekData"][0]["song"]["h"]["br"] = true;
            filter["weekData"][0]["song"]["h"]["size"] = true;
            filter["weekData"][0]["song"]["m"]["br"] = true;
            filter["weekData"][0]["song"]["m"]["size"] = true;
            filter["weekData"][0]["song"]["l"]["br"] = true;
            filter["weekData"][0]["song"]["l"]["size"] = true;
            filter["weekData"][0]["song"]["publishTime"] = true;
        }
        return commandRequest(url::user_record, token, data, filter);
    }
    class Netease
    {

        bool begin()
        {

            return true;
        }
    };

} // namespace netease
