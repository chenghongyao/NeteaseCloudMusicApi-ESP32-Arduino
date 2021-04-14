#include <Arduino.h>
#include <string.h>

#include "esp_system.h"
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"
#include "mbedtls/bignum.h"
#include "mbedtls/md5.h"

void printHex(const uint8_t* data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    Serial.printf("%02x ", data[i]);
    if ((i + 1) % 16 == 0) {
      Serial.printf("\n");
    }
  }
  Serial.printf("\n");
}

// void printChar(const char *data, size_t len) {
//   for (size_t i = 0; i < len; i++) {
//     Serial.printf("%c", data[i]);
//   }
//   Serial.printf("\n");
// }

namespace netease {

static const char nonce[] = "0CoJUm6Qyw8W8jud";
static const char modulus[] =
    "00e0b509f6259df8642dbc35662901477df22677ec152b5ff68ace615bb7b725152b3ab1"
    "7a876aea8a5aa76d2e417629ec4ee341f56135fccf695280104e0312ecbda92557c93870"
    "114af6c9d05c4f7f0c3685b7a46bee255932575cce10b424d813cfe4875d3e82047b97dd"
    "ef52741d546b8e289dc6935b3ece0462db0a22b8e7";
static const char pubKey[] = "010001";

static void createSecretKey(char* key, size_t size) {
  for (size_t i = 0; i < size; i++) {
    key[i] = esp_random() % 16;
    key[i] += (key[i] >= 10) ? 'a' - 10 : '0';
  }
}

static char* aesEncrypt(const char* data, const char* key) {
  uint8_t iv[] = "0102030405060708";
  size_t dataLen = strlen(data);
  uint8_t pad = 16 - dataLen % 16;
  uint8_t* textPadded = (uint8_t*)malloc(dataLen + pad);
  for (size_t i = 0; i < dataLen; i++) textPadded[i] = data[i];
  for (size_t i = 0; i < pad; i++) textPadded[dataLen++] = pad;

  uint8_t* cipher = (uint8_t*)malloc(dataLen);

  esp_aes_context ctx;
  mbedtls_aes_setkey_enc(&ctx, (uint8_t*)key, 128);
  mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, dataLen, iv, textPadded,
                        cipher);

  size_t olen;
  char* encResult = (char*)malloc(dataLen * 2 + 1);
  mbedtls_base64_encode((uint8_t*)encResult, dataLen * 2 + 1, &olen, cipher,
                        dataLen);

  encResult[dataLen * 2] = '\0';

  free(textPadded);
  free(cipher);

  return encResult;
}

static char* rsaEncrypt(const char* data, size_t dataLen) {
  static const size_t bufLen = 400;
  char* rsa = (char*)malloc(bufLen);

  char* dataHex = (char*)malloc(dataLen * 2 + 1);
  for (size_t i = 0; i < dataLen; i++) {
    dataHex[i * 2 + 0] = data[dataLen - i - 1] / 16;
    dataHex[i * 2 + 0] += (dataHex[i * 2 + 0] >= 10) ? 'a' - 10 : '0';

    dataHex[i * 2 + 1] = data[dataLen - i - 1] % 16;
    dataHex[i * 2 + 1] += (dataHex[i * 2 + 1] >= 10) ? 'a' - 10 : '0';
  }
  dataHex[dataLen * 2] = '\0';

  mbedtls_mpi mpiData;
  mbedtls_mpi mpiPubKey;
  mbedtls_mpi mpiModulus;
  mbedtls_mpi mpiResult;

  mbedtls_mpi_init(&mpiData);
  mbedtls_mpi_init(&mpiPubKey);
  mbedtls_mpi_init(&mpiModulus);
  mbedtls_mpi_init(&mpiResult);
  mbedtls_mpi_read_string(&mpiData, 16, dataHex);
  mbedtls_mpi_read_string(&mpiPubKey, 16, pubKey);
  mbedtls_mpi_read_string(&mpiModulus, 16, modulus);
  mbedtls_mpi_exp_mod(&mpiResult, &mpiData, &mpiPubKey, &mpiModulus, NULL);

  size_t olen;
  mbedtls_mpi_write_string(&mpiResult, 16, rsa, bufLen, &olen);

  mbedtls_mpi_free(&mpiData);
  mbedtls_mpi_free(&mpiPubKey);
  mbedtls_mpi_free(&mpiModulus);
  mbedtls_mpi_free(&mpiResult);
  free(dataHex);

  if (olen == 0 || olen > bufLen) {
    free(rsa);
    return NULL;

  } else {
    if (olen < 257) {
      char* pdst = rsa + 256;
      char* psrc = rsa + olen - 1;

      for (size_t i = 0; i < olen; i++) {
        *pdst-- = *psrc--;
      }

      for (size_t i = 0; i < 257 - olen; i++) {
        rsa[i] = '0';
      }
    }

    return rsa;
  }
}

String md5Encode(const String& text) {
  uint8_t md5[16];

  int ret = mbedtls_md5_ret((const uint8_t*)text.c_str(), text.length(), md5);
  if (ret != 0) return String();

  char t;
  String md5hex = "";
  for (size_t i = 0; i < 16; i++) {
    t = md5[i] / 16;
    t += (t >= 10) ? 'a' - 10 : '0';
    md5hex += t;

    t = md5[i] % 16;
    t += (t >= 10) ? 'a' - 10 : '0';
    md5hex += t;
  }

  return md5hex;
}

String urlEncode(const char* src) {
  String res;
  char t;

  while (*src) {
    if (*src == ' ') {
      res += '+';
    } else if ((*src >= '0' && *src <= '9') || (*src >= 'a' && *src <= 'z') ||
               (*src >= 'A' && *src <= 'Z') || (*src == '$') || (*src == '-') ||
               (*src == '.') || (*src == '!') || (*src == '*') ||
               (*src == '\'') || (*src == '(') || (*src == ')')) {
      res += *src;
    } else {
      res += '%';

      t = *src / 16;
      t += (t >= 10) ? 'a' - 10 : '0';
      res += t;

      t = *src % 16;
      t += (t >= 10) ? 'a' - 10 : '0';
      res += t;
    }
    src++;
  }
  return res;
}

// #include <StreamString.h>

String encryptRequest(const String& data) {
  char secKey[16];

  createSecretKey(secKey, 16);

  char* aesText = aesEncrypt(data.c_str(), nonce);
  if (aesText == NULL) return "";

  char* encText = aesEncrypt(aesText, secKey);
  if (encText == NULL) return "";

  char* encSecKey = rsaEncrypt(secKey, 16);
  if (encSecKey == NULL) return "";

  // url encode
  String res = "params=" + urlEncode(encText) + "&encSecKey=" + encSecKey;

  // char* bufOut = buffer;
  // strcpy(bufOut, "params=");
  // bufOut += sizeof("params=") - 1;
  // bufOut = urlEncode(encText, bufOut);

  // *bufOut++ = '&';
  // strcpy(bufOut, "encSecKey=");
  // bufOut += sizeof("encSecKey=") - 1;
  // strcpy(bufOut, encSecKey);

  free(aesText);
  free(encText);
  free(encSecKey);

  return res;
}

}  // namespace netease