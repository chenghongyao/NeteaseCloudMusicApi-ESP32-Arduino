#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "encrypt.hpp"

namespace netease {

    namespace url {

        const char *login =
            "http://music.163.com/weapi/login/cellphone?csrf_token=";
        const char *recommend_songs =
            "http://music.163.com/weapi/v2/discovery/recommend/songs?csrf_token=";
        const char *playlist_detail =
                "https://music.163.com/weapi/v3/playlist/detail";
        const char *user_playlist =
            "https://music.163.com/weapi/user/playlist";

    }


    DynamicJsonDocument commandRequest(const String &url,const String &token,
                                            const DynamicJsonDocument &data,const DynamicJsonDocument &filter,int bufferSize=1024*20){
        String strData;
        serializeJson(data, strData);

        DynamicJsonDocument doc(bufferSize);
        HTTPClient client;
        client.begin(url);
        client.addHeader("cookie", "MUSIC_U=" + token);
        client.addHeader("Content-Type", "application/x-www-form-urlencoded");
        client.setUserAgent(
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
            "like Gecko) Chrome/83.0.4103.97 Safari/537.36");

        int status_code = client.POST(netease::encryptRequest(strData));
        if (status_code != 200) {
            Serial.println(status_code);
        } else {
            DeserializationError error = deserializeJson(
                doc, client.getStream(), DeserializationOption::Filter(filter));
            if (error) {
            Serial.println("fail");
            Serial.println(error.f_str());
            }
        }
        client.end();
        return doc;
    }

    String login(const String &phone, const String &password) {
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
        if (status_code == 200) {
            DynamicJsonDocument doc(1024 * 5);
            deserializeJson(doc, client.getStream());
            int code = doc["code"].as<int>();
            if (code == 200) {
                res = doc["token"].as<String>();
            } else {
                Serial.print("code error:");
                Serial.println(code);
            }
        } else {
            Serial.print("http error:");
            Serial.println(status_code);
        }

        client.end();
        return res;
    }
    

    DynamicJsonDocument getRecommendSongs(const String &token) {
        DynamicJsonDocument data(100);
        data["offset"] = 0;
        data["limit"] = 999;
        data["total"] = true;


        DynamicJsonDocument filter(200);
        filter["code"] = true;
        filter["recommend"][0]["name"] = true;
        filter["recommend"][0]["id"] = true;
        filter["recommend"][0]["artists"][0]["name"] = true;
        filter["recommend"][0]["artists"][0]["id"] = true;
        filter["recommend"][0]["album"]["name"] = true;
        filter["recommend"][0]["album"]["id"] = true;

        return commandRequest(url::recommend_songs,token,data,filter);
    }
    
    DynamicJsonDocument getUserPlaylists(const String &token,const String &uid){

        DynamicJsonDocument data(100);
        data["offset"] = 0;
        data["limit"] = 99;
        data["uid"] = uid;

        DynamicJsonDocument filter(200);
        filter["code"] = true;
        filter["more"] = true;
        filter["playlist"][0]["name"] = true;
        filter["playlist"][0]["id"] = true;
        filter["playlist"][0]["userId"] = true;
        filter["playlist"][0]["trackCount"] = true;
        filter["playlist"][0]["playCount"] = true;

        return commandRequest(url::user_playlist,token,data,filter);
    }

    // TODO: start??
    DynamicJsonDocument getPlaylistDetail(const String &token,const String &id,int n){
        DynamicJsonDocument data(100);
        data["id"] = id;
        data["n"] = n;

        DynamicJsonDocument filter(300);
        filter["code"] = true;
        filter["playlist"]["name"] = true;
        filter["playlist"]["id"] = true;
        filter["playlist"]["coverImgUrl"] = true;
        filter["playlist"]["trackCount"] = true;
        filter["playlist"]["playCount"] = true;
        filter["playlist"]["tracks"][0]["name"] = true;
        filter["playlist"]["tracks"][0]["id"] = true;
        filter["playlist"]["tracks"][0]["ar"][0]["name"] = true;
        filter["playlist"]["tracks"][0]["ar"][0]["id"] = true;

        return commandRequest(url::playlist_detail,token,data,filter);
    }

    class Netease{

        bool begin(){
            

            return true;
        }

    };


}  // namespace netease
