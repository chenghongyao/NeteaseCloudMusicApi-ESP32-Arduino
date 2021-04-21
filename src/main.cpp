#include <Arduino.h>
#include <ArduinoJson.h>

#include "WiFi.h"
#include "SD_MMC.h"
#include "netease.hpp"

WiFiClient wifi_Client;
void setupWifi(const char *ssid, const char *passphrase)
{
  WiFi.begin(ssid, passphrase);
  while (!WiFi.isConnected())
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println(WiFi.localIP());
}

bool setupSD()
{
  if (!SD_MMC.begin())
  {
    Serial.println("sd mount fail");
    return false;
  }

  return true;
}

bool getLoginInfo(String &ssid, String &passphrase, String &phone, String &password)
{
  char buffer[200];

  File login_file = SD_MMC.open("/login.json");
  int file_size = login_file.size();
  if (file_size >= sizeof(buffer))
  {
    Serial.print("error:file size too large");
  }

  file_size = login_file.read((uint8_t *)buffer, file_size);
  buffer[file_size] = '\0';

  StaticJsonDocument<100> doc;
  deserializeJson(doc, buffer);
  phone = doc["phone"].as<const char *>();
  password = doc["password"].as<const char *>();
  ssid = doc["ssid"].as<const char *>();
  passphrase = doc["passphrase"].as<const char *>();

  return true;
}

void setup()
{
  Serial.begin(115200);

  String ssid, passphrase;
  String phone, password;
  if (!setupSD() || !getLoginInfo(ssid, passphrase, phone, password))
  {
    return;
  }

  Serial.println(ssid);
  Serial.println(passphrase);
  Serial.println(phone);
  Serial.println(password);

  setupWifi(ssid.c_str(), passphrase.c_str());

  String token = netease::login(phone, password);
  Serial.print("token:");
  Serial.println(token);

  // DynamicJsonDocument doc =  netease::getRecommendSongs(token);
  // serializeJsonPretty(doc, Serial);

  int uid;
  {
    DynamicJsonDocument doc = netease::getUserAccount(token);
    uid = doc["account"]["id"].as<int>();
    // serializeJsonPretty(doc, Serial);
  }
  Serial.println(uid);

  {
    DynamicJsonDocument doc = netease::getUserDetail(uid);
    // serializeJsonPretty(doc, Serial);
  }

  int pid;
  String pname;
  {
    DynamicJsonDocument doc = netease::getUserPlaylists(token, uid);
    pid = doc["playlist"][0]["id"].as<int>();
    pname = doc["playlist"][0]["name"].as<const char *>();
    // serializeJsonPretty(doc, Serial);
  }
  Serial.println(pid);
  Serial.println(pname);

  int mid;
  {
    DynamicJsonDocument doc = netease::getPlaylistDetail(token, pid, 50);
    mid = doc["playlist"]["tracks"][1]["id"].as<int>();
    // serializeJsonPretty(doc, Serial);
  }
  Serial.println(mid);

  String music_url;
  {
    DynamicJsonDocument doc = netease::getMusicUrl(mid);
    music_url = doc["data"][0]["url"].as<const char *>();
    // serializeJsonPretty(doc, Serial);
  }
  Serial.println(music_url);

  {
    // DynamicJsonDocument doc = netease::getUserRecord(29879272, 0);
    DynamicJsonDocument doc = netease::getUserRecord(uid, 1, token);
    // serializeJsonPretty(doc, Serial);
  }

  {
    DynamicJsonDocument doc = netease::getUserLevel(token);
    // serializeJsonPretty(doc, Serial);
  }

  {
    DynamicJsonDocument doc = netease::getPersonalFM(token);
    // serializeJsonPretty(doc, Serial);
  }
}

void loop()
{
}
