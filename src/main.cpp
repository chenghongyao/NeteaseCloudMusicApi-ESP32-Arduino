#include <Arduino.h>
#include <ArduinoJson.h>

#include "WiFi.h"
#include "SD_MMC.h"
#include "netease.h"

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

namespace nte = netease;
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

  auto me = netease::login(phone,password);
  Serial.println(me.token());

  auto resp = me.getUserAccount();
  serializeJsonPretty(resp,Serial);

}

void loop()
{
}
