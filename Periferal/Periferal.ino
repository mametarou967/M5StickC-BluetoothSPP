//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include <stdarg.h>
#include "BluetoothSerial.h"
#include <M5Unified.h> 

//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "ESP32-BT-Slave";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

void printBoth(bool ln,const char* format, ...) {
  va_list args;
  va_start(args, format);

  // フォーマットされた文字列を格納するためのバッファを作成
  char buffer[256]; // 適切なサイズを設定してください
  vsnprintf(buffer, sizeof(buffer), format, args);

  if(ln) println(buffer);
  else print(buffer);

  va_end(args);
}

void println(const char* str) {
  Serial.println(str);         // シリアルモニターにHello World!!と1行表示
  M5.Display.println(str);       // 画面に1行表示
}

void print(const char* str){
  Serial.print(str);         // シリアルモニターにHello World!!と1行表示
  M5.Display.print(str);       // 画面に1行表示
}

void setup() {
  auto cfg = M5.config();       // M5Stack初期設定用の構造体を代入
  M5.begin(cfg);                // M5デバイスの初期化
  // Serial.begin(115200);
  SerialBT.begin(device_name); //Bluetooth device name
  printBoth(false,"The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  // Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    printBoth(false,"%c",SerialBT.read());
    // Serial.write();
  }
  delay(20);
}
